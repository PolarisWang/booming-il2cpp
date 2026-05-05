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
    /// Emit a linear block of instructions with an optional terminator.
    /// Body instructions use EmitLinearInstruction (no goto-next).
    /// The terminator is emitted via dedicated helpers.
    /// </summary>
    private void EmitIRBlock(
        StringBuilder builder,
        IRBlock block,
        AotCoreIrMethodArtifact method,
        string indentation)
    {
        foreach (var instr in block.BodyInstructions)
            EmitLinearInstruction(builder, instr, indentation);

        if (block.Terminator != null)
            EmitIRBlockTerminator(builder, block.Terminator, method, indentation);
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
        StructuredIRNode ir = ConvertToStructuredIR(tree, cfg, instructions, offsets);
        EmitStructuredIRNode(builder, ir, method, "    ");
    }

    /// <summary>
    /// Bridge: convert emit-coupled StructuredNode tree to pure
    /// StructuredIR tree.
    /// </summary>
    private static StructuredIRNode ConvertToStructuredIR(
        StructuredNode node,
        ControlFlowGraph cfg,
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlySet<int> offsets)
    {
        switch (node)
        {
            case BasicBlockNode bn:
                return new IRBlock(bn.Block.BodyInstructions, bn.Block.Terminator);

            case SequenceNode sn:
            {
                var nodes = new List<StructuredIRNode>(sn.Nodes.Count);
                foreach (var child in sn.Nodes)
                    nodes.Add(ConvertToStructuredIR(child, cfg, instructions, offsets));
                return new IRSequence(nodes);
            }

            case IfThenElseNode ite:
            {
                var condBlock = ite.ConditionBlock;
                var thenIr = ConvertToStructuredIR(ite.ThenBranch, cfg, instructions, offsets);
                var elseIr = ite.ElseBranch != null
                    ? ConvertToStructuredIR(ite.ElseBranch, cfg, instructions, offsets)
                    : null;

                return new IRIfThenElse(
                    condBlock.BodyInstructions,
                    condBlock.Terminator ?? throw new InvalidOperationException(
                        "IfThenElse condition block must have a terminator"),
                    thenIr,
                    elseIr
                );
            }

            case LoopNode ln:
            {
                var header = ln.HeaderBlock;
                var bodyIr = ConvertToStructuredIR(ln.Body, cfg, instructions, offsets);

                if (ln.IsWhile)
                {
                    return new IRWhileLoop(
                        header.BodyInstructions,
                        header.Terminator,
                        bodyIr,
                        ln.ExitOffset ?? -1
                    );
                }
                else
                {
                    IReadOnlyList<AotCoreIrInstructionArtifact> latchInstructions;
                    AotCoreIrInstructionArtifact? latchTerminator;

                    if (ln.LatchBlock != null)
                    {
                        latchInstructions = ln.LatchBlock.BodyInstructions;
                        latchTerminator = ln.LatchBlock.Terminator;
                    }
                    else
                    {
                        latchInstructions = Array.Empty<AotCoreIrInstructionArtifact>();
                        latchTerminator = null;
                    }

                    return new IRDoWhileLoop(
                        bodyIr,
                        latchInstructions,
                        latchTerminator,
                        ln.HeaderBlock.StartOffset,
                        ln.ExitOffset ?? -1
                    );
                }
            }

            case SwitchNode sw:
            {
                var swBlock = sw.SwitchBlock;
                var caseBodies = new Dictionary<int, StructuredIRNode>();
                foreach (var kvp in sw.CaseBodies)
                {
                    caseBodies[kvp.Key] = ConvertToStructuredIR(kvp.Value, cfg, instructions, offsets);
                }
                var defaultBody = sw.DefaultBody != null
                    ? ConvertToStructuredIR(sw.DefaultBody, cfg, instructions, offsets)
                    : null;

                return new IRSwitch(
                    swBlock.BodyInstructions,
                    caseBodies,
                    defaultBody,
                    sw.MergeOffset ?? -1
                );
            }

            default:
                throw new NotSupportedException(
                    "ConvertToStructuredIR: unknown node type '" + node.GetType().Name + "'");
        }
    }
}
