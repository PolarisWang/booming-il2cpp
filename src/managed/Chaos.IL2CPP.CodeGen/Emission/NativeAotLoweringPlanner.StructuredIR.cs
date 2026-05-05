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
        private readonly List<int> _depths = new(); // stack of depth entries
        private int _maxDepth = 0;

        public int Depth => _depths.Count;
        public int MaxDepth => _maxDepth;

        /// <summary>Push a new slot, returning the C++ expression to assign it.</summary>
        public string Push()
        {
            _depths.Add(_depths.Count);
            _maxDepth = Math.Max(_maxDepth, _depths.Count);
            return $"__s[{_depths.Count - 1}]";
        }

        /// <summary>Pop and return the expression to read the top slot.</summary>
        public string Pop()
        {
            int depth = _depths.Count - 1;
            _depths.RemoveAt(depth);
            return $"__s[{depth}]";
        }

        /// <summary>Peek at the top slot expression without popping.</summary>
        public string Peek() => $"__s[{_depths.Count - 1}]";
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
                if (eqIdx < 0) continue;
                string prefix = line[..pushIdx].TrimEnd();
                string valueExpr = line[(eqIdx + 1)..].Trim().TrimEnd(';');
                // ldloca/ldarga produce pointer values (&chaos_locals[N] / &chaos_args[N])
                // Cast to CHAOS_IL2CPP_INTPTR since __s[N] is an integer type.
                string valueForSlot = (valueExpr.StartsWith("&chaos_locals") || valueExpr.StartsWith("&chaos_args"))
                    ? $"reinterpret_cast<CHAOS_IL2CPP_INTPTR>({valueExpr})"
                    : valueExpr;
                string slotExpr = slots.Push();
                lines[i] = $"{prefix}{slotExpr} = {valueForSlot};";
                continue;
            }

            // ── Pop:  ... = chaos_eval_stack[--chaos_stack_top];
            // Also handles return-value pop: return static_cast<T>(...[...]);
            // (return lines lack '=', so we match without the Contains('=') guard)
            int popIdx = line.IndexOf("chaos_eval_stack[--chaos_stack_top]", StringComparison.Ordinal);
            if (popIdx >= 0 && slots.Depth > 0)
            {
                string afterPop = line[(popIdx + "chaos_eval_stack[--chaos_stack_top]".Length)..];
                string slotName = slots.Pop();
                lines[i] = $"{line[..popIdx]}{slotName}{afterPop}";
                continue;
            }

            // ── Top modify: chaos_eval_stack[chaos_stack_top - 1] = VALUE;
            int topModIdx = line.IndexOf("chaos_eval_stack[chaos_stack_top - 1]", StringComparison.Ordinal);
            if (topModIdx >= 0)
            {
                int eqIdx = line.IndexOf('=', topModIdx);
                string prefix = line[..topModIdx].TrimEnd();
                string valueExpr = line[(eqIdx + 1)..].Trim().TrimEnd(';');
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
                string oldTop = slots.Peek(); // slot before push (will be at depth-1 after push)
                slots.Push(); // increment depth
                string newTop = slots.Peek(); // slot at new depth
                lines[i] = $"{prefix}{newTop} = {oldTop};";
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
    /// Goto target offsets collected from the current IR tree, used by
    /// EmitIRBlock to emit missing label definitions (chaos_ip_OFFSET:)
    /// for blocks that are targets of IRGoto or residual br/leave.
    /// </summary>
    private HashSet<int>? _irGotoTargets;

    /// <summary>
    /// Labels already emitted in the current method body (C++ labels have
    /// function scope, so duplicates across then/else bodies are illegal).
    /// </summary>
    private readonly HashSet<int> _emittedLabels = new();

    /// <summary>
    /// Recursively collect all goto/br/leave target offsets from the tree.
    /// </summary>
    private static void CollectGotoTargets(StructuredIRNode node, HashSet<int> targets)
    {
        switch (node)
        {
            case IRBlock block:
                if (block.Terminator != null &&
                    (block.Terminator.Op == "br" || block.Terminator.Op == "leave"))
                    targets.Add(GetRequiredIntOperand(block.Terminator));
                break;

            case IRSequence seq:
                foreach (var child in seq.Nodes)
                    CollectGotoTargets(child, targets);
                break;

            case IRIfThenElse ite:
                CollectGotoTargets(ite.ThenBody, targets);
                if (ite.ElseBody != null)
                    CollectGotoTargets(ite.ElseBody, targets);
                break;

            case IRWhileLoop w:
                CollectGotoTargets(w.Body, targets);
                break;

            case IRDoWhileLoop dw:
                CollectGotoTargets(dw.Body, targets);
                break;

            case IRSwitch sw:
                foreach (var caseBody in sw.CaseBodies.Values)
                    CollectGotoTargets(caseBody, targets);
                if (sw.DefaultBody != null)
                    CollectGotoTargets(sw.DefaultBody, targets);
                break;

            case IRGoto g:
                targets.Add(g.TargetOffset);
                break;

            case IRExceptionRegion er:
                CollectGotoTargets(er.TryBody, targets);
                CollectGotoTargets(er.HandlerBody, targets);
                break;

            // IRBreak, IRContinue, IRReturn, IRThrow — no target
        }
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
        // If this block's first instruction is a goto target, emit the label.
        // This covers both IRGoto and residual br/leave fallback targets.
        // C++ labels have function scope — guard against duplicates.
        if (_irGotoTargets != null && block.BodyInstructions.Count > 0)
        {
            int offset = GetRequiredIlOffset(block.BodyInstructions[0]);
            if (_irGotoTargets.Contains(offset) && _emittedLabels.Add(offset))
            {
                builder.AppendLine($"{indentation}chaos_ip_{offset}:");
            }
        }

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


    /// <summary>
    /// Compute the maximum eval-stack depth across all instructions in the method.
    /// This determines the size of the __s[] slot array.  Simulates the net
    /// push/pop effect of each IL opcode to find the peak concurrent depth.
    /// </summary>
    private static int ComputeMaxEvalStackDepth(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
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
                case "ldc.i4": case "ldc.i8": case "ldc.r4": case "ldc.r8":
                case "ldarg": case "ldstr": case "ldtoken": case "ldarga":
                case "ldnull": case "ldloc": case "ldloca":
                case "ldsfld": case "ldsflda":
                case "ldftn": case "newarr":
                    pushes = 1; pops = 0; break;

                // Dup: push a copy of the top value
                case "dup":
                    pushes = 1; pops = 0; break;

                // Pure pops (-1)
                case "pop": case "stloc": case "initobj":
                case "stsfld":
                case "throw":
                case "brfalse": case "brtrue":
                    pushes = 0; pops = 1; break;

                // Pops 2 (instance stores, indirect stores, conditional branches)
                case "stfld":
                case "stobj":
                case "stind.i4": case "stind.i1": case "stind.i2":
                case "stind.i8": case "stind.r4": case "stind.r8": case "stind.ref":
                case "beq": case "bgt": case "blt": case "bge": case "ble":
                case "bne.un": case "bge.un":
                    pushes = 0; pops = 2; break;

                // Pops 3
                case "stelem": case "stelem.ref":
                case "cpblk":
                    pushes = 0; pops = 3; break;

                // Pop 2, push 1 (net -1)
                case "cgt.un": case "ceq": case "cgt": case "clt":
                case "add": case "sub": case "mul": case "div": case "rem":
                case "shl": case "shr": case "shr.un":
                case "and": case "or": case "xor":
                case "add.ovf": case "sub.ovf": case "mul.ovf":
                case "ldelem": case "ldelem.ref": case "ldelema":
                    pushes = 1; pops = 2; break;

                // Pop 1, push 1 (net 0 — in-place transformation)
                case "ldfld": case "ldflda":
                case "ldind.i4": case "ldind.u1": case "ldind.i1":
                case "ldind.u2": case "ldind.i2": case "ldind.u4":
                case "ldind.i8": case "ldind.r4": case "ldind.r8": case "ldind.ref":
                case "box": case "unbox": case "unbox.any":
                case "castclass": case "isinst":
                case "ldobj": case "ldlen": case "localloc":
                case "conv.i4": case "conv.i1": case "conv.i2": case "conv.i8":
                case "conv.u8": case "conv.r4": case "conv.r8": case "conv.u":
                case "conv.u1": case "conv.u2":
                case "conv.ovf.i1": case "conv.ovf.u1":
                case "not":
                    pushes = 1; pops = 1; break;

                // Call/callvirt/calli: pop N args, push 0/1 result
                case "call": case "callvirt": case "calli":
                    pops = instr.TargetParameterCount ?? 0;
                    pushes = (instr.TargetReturnType != null && instr.TargetReturnType != "System.Void") ? 1 : 0;
                    break;

                // newobj: pop constructor args, push new object/value
                case "newobj":
                    pops = instr.TargetParameterCount ?? 0;
                    pushes = 1;
                    break;

                // ret: method return (depth resets)
                case "ret":
                    pushes = 0; pops = 0;
                    depth = 0;
                    break;

                // switch: pop index value
                case "switch":
                    pushes = 0; pops = 1; break;

                // br/leave: reset depth at unconditional branch target
                case "br": case "leave":
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

        return Math.Max(16, maxDepth);
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

        System.Console.Error.WriteLine("TRACE:Em " + (method.SubjectId != null && method.SubjectId.Length > 80 ? method.SubjectId.Substring(0, 80) : method.SubjectId ?? "null") + " instr=" + instructions.Count);
        var cfg = BuildControlFlowGraph(instructions, offsets);
        if (!cfg.IsReducible)
        {
            // Fallback: flat goto mode (emits chaos_ip_OFFSET: labels + goto-next)
            // TODO: replace with node-splitting in Phase 2 when IR builder is rewritten
            EmitInstructionRange(builder, method, instructions, nextOffsetsByIlOffset, offsets);
            return;
        }

        // Slot array for structured-IR eval-stack replacement.
        // Each IRBlock's ReplaceStackOpsWithSlots rewrites
        // chaos_eval_stack[...] patterns to __s[N], so declare the
        // backing store here for the whole method body.
        int maxDepth = ComputeMaxEvalStackDepth(instructions);
        builder.AppendLine($"    CHAOS_IL2CPP_INTPTR __s[{maxDepth}] = {{}};");

        // Check for exception regions — route through IRExceptionRegion path for
        // simple shapes (catch-only, filter-only, finally-only) that would
        // otherwise go through the old EmitCatchOnlyExceptionMethodBody etc.
        if (method.ExceptionRegionCount > 0 && method.ExceptionRegions is { Count: > 0 })
        {
            EmitViaStructuredIRWithExceptions(builder, method, instructions, nextOffsetsByIlOffset, offsets);
            return;
        }

        var tree = RecoverStructure(cfg, 0, cfg.Blocks.Count - 1);
        StructuredIRNode ir = tree;

        // Collect ALL branch target offsets — both from the raw instruction
        // list (EnumerateBranchTargets) and from any IRGoto nodes the structure
        // recovery introduced.  This guarantees every goto chaos_ip_N has a
        // matching label regardless of how the IR tree expresses branches.
        var gotoTargets = EnumerateBranchTargets(instructions);
        CollectGotoTargets(ir, gotoTargets);
        _irGotoTargets = gotoTargets;
        _emittedLabels.Clear();
        try
        {
            EmitStructuredIRNode(builder, ir, method, "    ");
        }
        finally
        {
            _irGotoTargets = null;
        }
    }

    /// <summary>
    /// Handle exception-region methods through the IRExceptionRegion path.
    /// Partitions the instruction list using the existing TryCreate* shape
    /// detection and builds a StructuredIR tree with IRExceptionRegion nodes.
    /// Complex shapes (CatchAndFinally, FilterAndFinally) fall back to the
    /// old exception emission path via EmitCatchOnlyExceptionMethodBody etc.
    /// </summary>
    private void EmitViaStructuredIRWithExceptions(
        StringBuilder builder,
        AotCoreIrMethodArtifact method,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlyDictionary<int, int?> nextOffsetsByIlOffset,
        IReadOnlySet<int> offsets)
    {
        StructuredIRNode? body = null;

        if (TryCreateCatchOnlyExceptionMethodShape(method, out var catchOnly))
        {
            body = BuildExceptionIRBody(
                catchOnly.PrefixInstructions,
                catchOnly.TryInstructions,
                catchOnly.HandlerInstructions,
                catchOnly.TailInstructions,
                IRExceptionKind.TryCatch,
                offsets,
                catchTypeSubjectId: catchOnly.ExceptionRegion.CatchTypeSubjectId);
        }
        else if (TryCreateFilterOnlyExceptionMethodShape(method, out var filterOnly))
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
        }
        else if (TryCreateFinallyOnlyExceptionMethodShape(method, out var finallyOnly))
        {
            // Handle the common case of a single finally handler.
            // Multiple finally handlers fall back to flat goto.
            if (finallyOnly.FinallyHandlers.Count == 1)
            {
                body = BuildExceptionIRBody(
                    finallyOnly.PrefixInstructions,
                    finallyOnly.TryInstructions,
                    finallyOnly.FinallyHandlers[0].Instructions,
                    finallyOnly.TailInstructions,
                    IRExceptionKind.TryFinally,
                    offsets);
            }
        }

        if (body == null)
        {
            // Fallback: flat goto for unrecognized exception shapes
            EmitInstructionRange(builder, method, instructions, nextOffsetsByIlOffset, offsets);
            return;
        }

        var gotoTargets = EnumerateBranchTargets(instructions);
        CollectGotoTargets(body, gotoTargets);
        _irGotoTargets = gotoTargets;
        _emittedLabels.Clear();
        try
        {
            EmitStructuredIRNode(builder, body, method, "    ");
        }
        finally
        {
            _irGotoTargets = null;
        }
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

        var subOffsets = new HashSet<int>(instructions.Select(GetRequiredIlOffset));
        var cfg = BuildControlFlowGraph(instructions, subOffsets);
        if (!cfg.IsReducible)
            return new IRSequence(Array.Empty<StructuredIRNode>());

        return RecoverStructure(cfg, 0, cfg.Blocks.Count - 1);
    }

}
