using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

    /// <summary>
    /// Post-pass that converts residual br/leave terminators in a structured IR
    /// tree into IRBreak, IRContinue, or IRReturn where possible.
    ///
    /// This pass runs AFTER RecoverStructure and eliminates branches that could
    /// not be mapped to structured control flow during initial recovery.
    ///
    /// - br targeting a loop exit → IRBreak
    /// - br targeting a loop header → IRContinue
    /// - br targeting a forward block → strip terminator (implicit fallthrough)
    /// - leave targeting a known exit → IRBreak (for structured EH boundaries)
    /// - Unrecognized br/leave → kept as-is (safety net; should not occur after
    ///   interval-based CFG restructuring)
    /// </summary>
    private static StructuredIRNode ConvertResidualBranches(
        StructuredIRNode node,
        IReadOnlySet<int>? loopExitOffsets,
        int? loopHeaderOffset)
    {
        switch (node)
        {
            case IRBlock block when block.Terminator is { Op: "br" or "leave" }:
                {
                    int target = GetRequiredIntOperand(block.Terminator);
                    bool isLeave = block.Terminator.Op == "leave";

                    // Check for break (targets a loop exit)
                    if (loopExitOffsets != null && loopExitOffsets.Contains(target))
                        return new IRBreak();

                    // Check for continue (targets the loop header — backedge)
                    if (loopHeaderOffset.HasValue && target == loopHeaderOffset.Value)
                        return new IRContinue();

                    // Forward branch to a block after this one — safe to strip
                    // (structured control flow provides implicit fallthrough).
                    if (!isLeave && target >= 0)
                        return new IRBlock(block.BodyInstructions, null);

                    // leave that is not a loop break: keep terminator
                    return node;
                }

            case IRSequence seq when seq.Nodes.Count > 0:
                {
                    bool changed = false;
                    var newNodes = new List<StructuredIRNode>(seq.Nodes.Count);
                    foreach (var n in seq.Nodes)
                    {
                        var converted = ConvertResidualBranches(n, loopExitOffsets, loopHeaderOffset);
                        if (!ReferenceEquals(converted, n)) changed = true;
                        newNodes.Add(converted);
                    }
                    return changed ? new IRSequence(newNodes) : seq;
                }

            case IRIfThenElse ite:
                {
                    var thenBody = ConvertResidualBranches(ite.ThenBody, loopExitOffsets, loopHeaderOffset);
                    var elseBody = ite.ElseBody is null ? null : ConvertResidualBranches(ite.ElseBody, loopExitOffsets, loopHeaderOffset);
                    var postMerge = ite.PostMergeBody is null ? null : ConvertResidualBranches(ite.PostMergeBody, loopExitOffsets, loopHeaderOffset);
                    if (!ReferenceEquals(thenBody, ite.ThenBody) ||
                        !ReferenceEquals(elseBody, ite.ElseBody) ||
                        !ReferenceEquals(postMerge, ite.PostMergeBody))
                    {
                        return new IRIfThenElse(ite.ConditionInstructions, ite.BranchTerminator,
                            thenBody, elseBody, postMerge, ite.PreConditionDepth);
                    }
                    return ite;
                }

            case IRWhileLoop loop:
                {
                    // Build exit set: original exit + the loop's exit offset
                    var innerExits = new HashSet<int>(loopExitOffsets ?? new HashSet<int>());
                    if (loop.ExitOffset >= 0) innerExits.Add(loop.ExitOffset);
                    int innerHeader = loop.ConditionInstructions.Count > 0
                        ? (loop.ConditionInstructions[0].IlOffset)  // approximate header offset
                        : -1;

                    // Use the header's start offset from the condition instructions if available
                    int? innerHeaderOffset = loopHeaderOffset;

                    var body = ConvertResidualBranches(loop.Body, innerExits, innerHeaderOffset);
                    if (!ReferenceEquals(body, loop.Body))
                        return new IRWhileLoop(loop.ConditionInstructions, loop.ConditionTerminator, body, loop.ExitOffset);
                    return loop;
                }

            case IRDoWhileLoop loop:
                {
                    var innerExits = new HashSet<int>(loopExitOffsets ?? new HashSet<int>());
                    if (loop.ExitOffset >= 0) innerExits.Add(loop.ExitOffset);
                    var body = ConvertResidualBranches(loop.Body, innerExits, loop.HeaderOffset);
                    if (!ReferenceEquals(body, loop.Body))
                        return new IRDoWhileLoop(body, loop.LatchInstructions, loop.LatchTerminator, loop.HeaderOffset, loop.ExitOffset);
                    return loop;
                }

            case IRSwitch sw:
                {
                    bool changed = false;
                    var caseBodies = new Dictionary<int, StructuredIRNode>();
                    foreach (var (caseValue, caseBody) in sw.CaseBodies)
                    {
                        var converted = ConvertResidualBranches(caseBody, loopExitOffsets, loopHeaderOffset);
                        if (!ReferenceEquals(converted, caseBody)) changed = true;
                        caseBodies[caseValue] = converted;
                    }
                    var defaultBody = sw.DefaultBody is null ? null : ConvertResidualBranches(sw.DefaultBody, loopExitOffsets, loopHeaderOffset);
                    if (changed || !ReferenceEquals(defaultBody, sw.DefaultBody))
                        return new IRSwitch(sw.SwitchInstructions, caseBodies, defaultBody, sw.ExitOffset, sw.FallthroughCaseValues);
                    return sw;
                }

            case IRExceptionRegion er:
                {
                    var tryBody = ConvertResidualBranches(er.TryBody, loopExitOffsets, loopHeaderOffset);
                    var handlerBody = ConvertResidualBranches(er.HandlerBody, loopExitOffsets, loopHeaderOffset);
                    if (!ReferenceEquals(tryBody, er.TryBody) || !ReferenceEquals(handlerBody, er.HandlerBody))
                        return new IRExceptionRegion(er.Kind, tryBody, handlerBody, er.CatchTypeSubjectId, er.FilterInstructions);
                    return er;
                }

            default:
                return node;
        }
    }



    /// <summary>
    /// Recover structured control flow from the CFG as a pure StructuredIR tree.
    /// </summary>
    private static StructuredIRNode RecoverStructure(
        ControlFlowGraph cfg,
        int startIndex, int endIndex,
        int? loopHeaderOffset = null,
        IReadOnlySet<int>? loopExitOffsets = null,
        int depth = 0)
    {
        // Note: RuntimeHelpers.TryEnsureSufficientExecutionStack is intentionally
        // NOT used here. .NET's runtime stack guard is based on a fixed per-thread
        // limit (~4 MB from dotnet.exe/AppHost), not the actual OS stack size.
        // Using it causes premature fallback to flat IR on any thread, regardless
        // of how much OS stack remains. Instead we rely on the static depth counter
        // which has been empirically tuned (depth=5 safe on 4 MB main thread).
        if (depth > MaxRecoverStructureDepth)
            return new IRSequence(Array.Empty<StructuredIRNode>());

        if (startIndex > endIndex)
            return new IRSequence(Array.Empty<StructuredIRNode>());

        if (startIndex == endIndex)
        {
            var block = cfg.Blocks[startIndex];

            // Check for break/continue targeting the surrounding loop context
            if (block.Terminator != null && (block.Terminator.Op is "br" or "leave"))
            {
                int target = GetRequiredIntOperand(block.Terminator);
                if (loopExitOffsets != null && loopExitOffsets.Contains(target))
                    return new IRBreak();
                if (loopHeaderOffset.HasValue && target == loopHeaderOffset.Value)
                {
                    // Backedge (latch block): emit body instructions followed by continue.
                    if (block.BodyInstructions.Count > 0)
                    {
                        return new IRSequence(new StructuredIRNode[]
                        {
                            new IRBlock(block.BodyInstructions, null),
                            new IRContinue()
                        });
                    }
                    return new IRContinue();
                }
            }

            if (block.Terminator == null || block.IsTerminal || block.Terminator.Op is "br" or "leave")
            {
                return new IRBlock(block.BodyInstructions, block.Terminator);
            }
            if (IsConditionalBranchOpcode(block.Terminator.Op))
            {
                return BuildIfThenElse(cfg, startIndex, endIndex, loopHeaderOffset, loopExitOffsets, depth + 1);
            }
            if (IsSwitchOpcode(block.Terminator.Op))
            {
                return BuildSwitch(cfg, startIndex, loopHeaderOffset, loopExitOffsets, depth + 1);
            }
            return new IRBlock(block.BodyInstructions, block.Terminator);
        }

        // Check if startIndex is a loop header
        if (cfg.LoopHeaders.TryGetValue(startIndex, out var loopInfo))
        {
            return BuildLoop(cfg, startIndex, endIndex, loopInfo, loopHeaderOffset, loopExitOffsets, depth + 1);
        }

        // Try to find a conditional branch at the start of the interval
        var firstBlock = cfg.Blocks[startIndex];
        if (firstBlock.ConditionalTarget.HasValue && IsConditionalBranchOpcode(firstBlock.Terminator?.Op ?? ""))
        {
            // Self-loop guard: conditional branch targets its own block.
            // BuildIfThenElse would call RecoverStructure for the true branch which
            // hits the same block with the same conditional branch -> infinite recursion.
            if (cfg.OffsetToBlockIndex.TryGetValue(firstBlock.ConditionalTarget.Value, out var selfIdx) &&
                selfIdx == startIndex)
            {
                // Fall through to sequential block handling below; block will be emitted as flat IRBlock.
            }
            else
            {
                var ite = BuildIfThenElse(cfg, startIndex, endIndex, loopHeaderOffset, loopExitOffsets, depth + 1);
                if (ite is IRIfThenElse)
                {
                    // Determine merge point from CFG
                    int trueBlockIdx = cfg.OffsetToBlockIndex[firstBlock.ConditionalTarget.Value];
                    int falseBlockIdx = startIndex + 1;
                    int? mergeOffset = FindMergePoint(cfg, startIndex, trueBlockIdx, falseBlockIdx);

                    int afterMergeIdx;
                    if (ite is IRIfThenElse iteResult && iteResult.PostMergeBody != null)
                    {
                        // PostMergeBody absorbed all code from merge point to endIndex.
                        afterMergeIdx = endIndex + 1;
                    }
                    else if (mergeOffset.HasValue &&
                             cfg.OffsetToBlockIndex.TryGetValue(mergeOffset.Value, out var mergeIdx))
                    {
                        afterMergeIdx = mergeIdx + 1;
                    }
                    else
                    {
                        afterMergeIdx = endIndex + 1;
                    }

                    var nodes = new List<StructuredIRNode> { ite };
                    if (afterMergeIdx <= endIndex)
                    {
                        nodes.Add(RecoverStructure(cfg, afterMergeIdx, endIndex, loopHeaderOffset, loopExitOffsets, depth + 1));
                    }
                    return new IRSequence(nodes);
                }
            }
        }

        // Try to find a switch at the start
        if (IsSwitchOpcode(firstBlock.Terminator?.Op ?? ""))
        {
            var sw = BuildSwitch(cfg, startIndex, loopHeaderOffset, loopExitOffsets);
            if (sw is IRSwitch swNode)
            {
                // Recover post-switch code starting at the exit offset block.
                int nextIdx = endIndex + 1;
                int exitOffset = swNode.ExitOffset;
                if (exitOffset >= 0 && cfg.OffsetToBlockIndex.TryGetValue(exitOffset, out var exitBlockIdx))
                    nextIdx = exitBlockIdx;

                int afterMergeIdx = nextIdx <= endIndex ? nextIdx : endIndex + 1;

                var nodes = new List<StructuredIRNode> { swNode };
                if (afterMergeIdx <= endIndex)
                {
                    nodes.Add(RecoverStructure(cfg, afterMergeIdx, endIndex, loopHeaderOffset, loopExitOffsets, depth + 1));
                }
                return new IRSequence(nodes);
            }
        }

        // Otherwise: sequential blocks
        var seqNodes = new List<StructuredIRNode>();
        int idx = startIndex;
        while (idx <= endIndex)
        {
            var block = cfg.Blocks[idx];

            if (cfg.LoopHeaders.TryGetValue(idx, out var currentLoop))
            {
                var loop = BuildLoop(cfg, idx, endIndex, currentLoop, loopHeaderOffset, loopExitOffsets, depth + 1);
                seqNodes.Add(loop);

                // Emit any exit blocks that sit between the loop header and
                // the first body block (e.g. early-return before the latch).
                var afterHeaderBody = currentLoop.BodyIndices.Where(i => i > idx).ToList();
                int firstBodyIdx = afterHeaderBody.Count > 0 ? afterHeaderBody.Min() : idx + 1;
                for (int e = idx + 1; e < firstBodyIdx && e <= endIndex; e++)
                    seqNodes.Add(new IRBlock(cfg.Blocks[e].BodyInstructions, cfg.Blocks[e].Terminator));

                // Use BodyIndices.Max() instead of maxLatch so ALL loop body blocks
                // are consumed — not just those up to the max latch.  A body block
                // can exist after the max latch when a conditional latch falls through
                // to non-latch body code.
                int loopBodyMax = currentLoop.BodyIndices.Max();
                idx = loopBodyMax + 1 > endIndex ? endIndex + 1 : loopBodyMax + 1;
            }
            else if (block.ConditionalTarget.HasValue && IsConditionalBranchOpcode(block.Terminator?.Op ?? ""))
            {
                // Self-loop guard: conditional branch targets its own block.
                if (cfg.OffsetToBlockIndex.TryGetValue(block.ConditionalTarget.Value, out var seqSelfIdx) &&
                    seqSelfIdx == idx)
                {
                    seqNodes.Add(new IRBlock(block.BodyInstructions, block.Terminator));
                    idx++;
                    continue;
                }
                var ite = BuildIfThenElse(cfg, idx, endIndex, loopHeaderOffset, loopExitOffsets, depth + 1);
                seqNodes.Add(ite);
                if (ite is IRIfThenElse iteNode && iteNode.PostMergeBody != null)
                {
                    // PostMergeBody absorbed all code from merge to endIndex.
                    idx = endIndex + 1;
                }
                else if (ite is IRIfThenElse)
                {
                    int trueBlockIdx2 = cfg.OffsetToBlockIndex[block.ConditionalTarget.Value];
                    int falseBlockIdx2 = idx + 1;
                    int? mergeOff = FindMergePoint(cfg, idx, trueBlockIdx2, falseBlockIdx2);
                    if (mergeOff.HasValue && cfg.OffsetToBlockIndex.TryGetValue(mergeOff.Value, out var mIdx))
                        idx = mIdx + 1;
                    else
                        idx = endIndex + 1;
                }
                else
                {
                    idx = endIndex + 1;
                }
            }
            else if (IsSwitchOpcode(block.Terminator?.Op ?? ""))
            {
                var sw = BuildSwitch(cfg, idx, loopHeaderOffset, loopExitOffsets);
                seqNodes.Add(sw);
                if (sw is IRSwitch sw2)
                {
                    int swNextIdx = endIndex + 1;
                    int exitOffset = sw2.ExitOffset;
                    if (exitOffset >= 0 && cfg.OffsetToBlockIndex.TryGetValue(exitOffset, out var exitBlockIdx))
                        swNextIdx = exitBlockIdx;
                    idx = swNextIdx <= endIndex ? swNextIdx : endIndex + 1;
                }
                else
                {
                    idx = endIndex + 1;
                }
            }
            else
            {
                // Check if this block belongs to a pending reversed loop
                // (body blocks before header in layout). Skip it here;
                // BuildLoop at the loop header will include it.
                bool isPendingReversedLoopBody = false;
                for (int scanIdx = idx + 1; scanIdx <= endIndex; scanIdx++)
                {
                    if (cfg.LoopHeaders.TryGetValue(scanIdx, out var revLoop) &&
                        revLoop.BodyIndices.Contains(idx))
                    {
                        isPendingReversedLoopBody = true;
                        break;
                    }
                }
                if (isPendingReversedLoopBody)
                {
                    idx++;
                    continue;
                }

                // Check for break/continue in sequential block
                var seqBlock = block;
                if (seqBlock.Terminator != null && (seqBlock.Terminator.Op is "br" or "leave"))
                {
                    int seqTarget = GetRequiredIntOperand(seqBlock.Terminator);
                    if (loopExitOffsets != null && loopExitOffsets.Contains(seqTarget))
                    {
                        seqNodes.Add(new IRBreak());
                        idx++;
                        continue;
                    }
                    if (loopHeaderOffset.HasValue && seqTarget == loopHeaderOffset.Value)
                    {
                        seqNodes.Add(new IRContinue());
                        idx++;
                        continue;
                    }
                    // If the br/leave targets a later block within this range,
                    // it's a sequential fallthrough — strip the terminator.
                    if (cfg.OffsetToBlockIndex.TryGetValue(seqTarget, out var fallthroughIdx) &&
                        fallthroughIdx > idx && fallthroughIdx <= endIndex)
                    {
                        seqNodes.Add(new IRBlock(seqBlock.BodyInstructions, null));
                        idx++;
                        continue;
                    }
                }

                seqNodes.Add(new IRBlock(seqBlock.BodyInstructions, seqBlock.Terminator));
                idx++;
            }
        }

        return seqNodes.Count == 1 ? seqNodes[0] : new IRSequence(seqNodes);
    }



    /// <summary>
    /// Build a loop node from a natural loop.
    /// </summary>
    private static StructuredIRNode BuildLoop(
        ControlFlowGraph cfg,
        int headerIndex, int endIndex,
        NaturalLoopInfo loopInfo,
        int? outerLoopHeaderOffset = null,
        IReadOnlySet<int>? outerLoopExitOffsets = null,
        int depth = 0)
    {
        var header = cfg.Blocks[headerIndex];
        var bodyIndicesAfterHeader = loopInfo.BodyIndices.Where(i => i > headerIndex).ToList();

        // Detect reversed loop pattern (C# for-loop IL: body before condition).
        // In this pattern, the header (condition block) comes AFTER some body blocks.
        bool isReversed = bodyIndicesAfterHeader.Count == 0 && loopInfo.BodyIndices.Any(i => i < headerIndex);

        int bodyStart, bodyEnd, exitIdx;
        bool isWhile;
        BasicBlock? latchBlock = null;

        if (isReversed)
        {
            // Reversed loop: body blocks precede the header in layout.
            // The header's condition block is the last thing in the loop body,
            // and the backward edge from header→body is the "continue" edge.
            // This is inherently a do-while pattern.
            int minBody = loopInfo.BodyIndices.Min();
            int maxBody = loopInfo.BodyIndices.Max();
            bodyStart = minBody;
            bodyEnd = headerIndex - 1; // body up to (not including) the header

            exitIdx = headerIndex + 1;
            isWhile = false;
            latchBlock = header; // the header (condition block) serves as latch
        }
        else
        {
            bodyStart = bodyIndicesAfterHeader.Count > 0 ? bodyIndicesAfterHeader.Min() : headerIndex + 1;

            int maxLatchIdx = loopInfo.LatchIndices.Max();
            bodyEnd = Math.Max(maxLatchIdx, loopInfo.BodyIndices.Max());
            if (bodyEnd > endIndex) bodyEnd = endIndex;

            exitIdx = bodyEnd + 1;
            while (exitIdx <= endIndex && loopInfo.BodyIndices.Contains(exitIdx))
                exitIdx++;

            if (loopInfo.LatchIndices.Contains(headerIndex))
            {
                isWhile = false;
                latchBlock = header;
            }
            else if (header.ConditionalTarget.HasValue && IsConditionalBranchOpcode(header.Terminator?.Op ?? ""))
            {
                isWhile = true;
                var latchesInBody = loopInfo.LatchIndices.Where(l => l <= bodyEnd && l >= bodyStart).ToList();
                if (latchesInBody.Count > 0)
                {
                    int latchIdx = latchesInBody.Max();
                    latchBlock = cfg.Blocks[latchIdx];
                }
            }
            else
            {
                isWhile = false;
                var latchesInBody = loopInfo.LatchIndices.Where(l => l <= bodyEnd && l >= bodyStart).ToList();
                if (latchesInBody.Count > 0)
                {
                    int latchIdx = latchesInBody.Max();
                    latchBlock = cfg.Blocks[latchIdx];
                }
            }
        }

        // Build exit offset set: merge natural exit + early exits + outer loop exits
        var loopExitSet = new HashSet<int>(outerLoopExitOffsets ?? new HashSet<int>());
        if (outerLoopExitOffsets != null)
        {
            foreach (var o in outerLoopExitOffsets)
                loopExitSet.Add(o);
        }

        // Build body with the loop's own context for break/continue detection
        StructuredIRNode body;
        if (bodyStart <= bodyEnd)
            body = RecoverStructure(cfg, bodyStart, bodyEnd, outerLoopHeaderOffset, loopExitSet, depth + 1);
        else
            body = new IRSequence(Array.Empty<StructuredIRNode>());

        int exitVal = exitIdx;

        if (isWhile)
        {
            return new IRWhileLoop(
                header.BodyInstructions,
                header.Terminator,
                body,
                exitVal);
        }
        else
        {
            IReadOnlyList<AotCoreIrInstructionArtifact> latchInstructions;
            AotCoreIrInstructionArtifact? latchTerminator;

            if (latchBlock != null)
            {
                latchInstructions = latchBlock.BodyInstructions;
                latchTerminator = latchBlock.Terminator;
            }
            else
            {
                latchInstructions = Array.Empty<AotCoreIrInstructionArtifact>();
                latchTerminator = null;
            }

            return new IRDoWhileLoop(
                body,
                latchInstructions,
                latchTerminator,
                outerLoopHeaderOffset ?? -1,
                exitVal);
        }
    }



    /// <summary>
    /// Computes the maximum expected eval-stack depth at the start of the
    /// condition block based on predecessor blocks' net stack contributions.
    /// Values pushed by predecessor CFG blocks are invisible to the structured
    /// slot tracker when it processes the condition instructions, so this
    /// computed depth ensures the slot context has adequate capacity.
    /// </summary>
    private static int ComputePreConditionDepth(ControlFlowGraph cfg, int condBlockIndex)
    {
        var condBlock = cfg.Blocks[condBlockIndex];
        int condStartOffset = condBlock.StartOffset;
        int maxDepth = 0;

        for (int i = 0; i < cfg.Blocks.Count; i++)
        {
            if (i == condBlockIndex)
                continue;

            var block = cfg.Blocks[i];

            // Check if this block can reach the condBlock via an edge.
            bool isPredecessor = false;
            if (block.BranchTarget.HasValue && block.BranchTarget.Value == condStartOffset)
                isPredecessor = true;
            else if (block.ConditionalTarget.HasValue && block.ConditionalTarget.Value == condStartOffset)
                isPredecessor = true;
            else if (block.SwitchTargets.Contains(condStartOffset))
                isPredecessor = true;
            else if ((block.Terminator == null || !IsBlockTerminatorOpcode(block.Terminator.Op))
                     && i + 1 < cfg.Blocks.Count
                     && cfg.Blocks[i + 1].StartOffset == condStartOffset)
                isPredecessor = true;

            if (!isPredecessor)
                continue;

            // Compute net stack contribution of this predecessor block.
            int netPushes = 0;
            foreach (var instr in block.BodyInstructions)
            {
                netPushes += EstimatePushCount(instr.Op);
                netPushes -= EstimatePopCount(instr.Op);
            }
            if (block.Terminator != null)
                netPushes -= EstimateTerminatorPopCount(block.Terminator.Op);

            if (netPushes > maxDepth)
                maxDepth = netPushes;
        }

        return maxDepth;
    }



    /// <summary>
    /// Build an if-then-else node starting from a conditional branch block.
    /// </summary>



    /// <summary>
    /// Build a switch node.
    /// </summary>



    // ────────────────────────────────────────────────────────────────────────────
    // Node Splitting — make irreducible CFGs reducible
    // ────────────────────────────────────────────────────────────────────────────

    /// <summary>
    /// Attempt to make an irreducible CFG reducible by node splitting.
    /// For each block that receives backward edges without being a proper loop
    /// header, create a copy and redirect all forward (external) predecessors
    /// to the copy, so the original block becomes a single-entry loop header.
    /// </summary>



    /// <summary>
    /// Find blocks that are the target of at least one unnatural backward branch
    /// (backward edge where the target is not a loop header, or the source is not a latch).
    /// </summary>

}
