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
    /// Build an if-then-else node starting from a conditional branch block.
    /// </summary>
    private static StructuredIRNode BuildIfThenElse(
        ControlFlowGraph cfg, int conditionBlockIndex,
        int endIndex,
        int? loopHeaderOffset = null, IReadOnlySet<int>? loopExitOffsets = null,
        int depth = 0)
    {
        var condBlock = cfg.Blocks[conditionBlockIndex];
        if (!condBlock.ConditionalTarget.HasValue)
            return new IRBlock(condBlock.BodyInstructions, condBlock.Terminator);

        int preCondDepth = ComputePreConditionDepth(cfg, conditionBlockIndex);

        var trueTarget = condBlock.ConditionalTarget.Value;
        if (!cfg.OffsetToBlockIndex.TryGetValue(trueTarget, out var trueBlockIdx))
            return new IRBlock(condBlock.BodyInstructions, condBlock.Terminator);

        int falseBlockIdx = conditionBlockIndex + 1;
        int endIdx = endIndex;

        int? mergeOffset = FindMergePoint(cfg, conditionBlockIndex, trueBlockIdx, falseBlockIdx);
        int mergeIdx = -1;

        StructuredIRNode thenBranch;
        StructuredIRNode? elseBranch;
        int? effectiveMerge = null;

        if (mergeOffset.HasValue && cfg.OffsetToBlockIndex.TryGetValue(mergeOffset.Value, out mergeIdx))
        {
            thenBranch = RecoverStructure(cfg, trueBlockIdx, mergeIdx - 1, loopHeaderOffset, loopExitOffsets, depth + 1);
            // Else range must not overlap with the true-target block.
            int elseEnd = Math.Min(mergeIdx - 1, trueBlockIdx - 1);
            if (falseBlockIdx <= elseEnd)
            {
                elseBranch = RecoverStructure(cfg, falseBlockIdx, elseEnd, loopHeaderOffset, loopExitOffsets, depth + 1);
            }
            else
            {
                elseBranch = null;
            }
            effectiveMerge = mergeOffset;
        }
        else
        {
            thenBranch = RecoverStructure(cfg, trueBlockIdx, endIdx, loopHeaderOffset, loopExitOffsets, depth + 1);
            // Else range must not overlap with the true-target block.
            int elseEnd2 = Math.Min(endIdx, trueBlockIdx - 1);
            if (falseBlockIdx <= elseEnd2)
                elseBranch = RecoverStructure(cfg, falseBlockIdx, elseEnd2, loopHeaderOffset, loopExitOffsets, depth + 1);
            else
                elseBranch = null;
        }

        // Eliminate trailing br/leave that target the merge point (fallthrough is implicit)
        if (effectiveMerge.HasValue)
        {
            thenBranch = RemoveTrailingBranch(thenBranch, effectiveMerge.Value);
            if (elseBranch != null)
                elseBranch = RemoveTrailingBranch(elseBranch, effectiveMerge.Value);
        }

        // Recover the merge block and all subsequent code as PostMergeBody.
        // This naturally handles nested ITEs (merge block with conditional branch),
        // sequential continuation code, and terminal blocks — without the caller
        // skipping the merge block via mergeIdx + 1.
        StructuredIRNode? postMergeBody = null;
        if (effectiveMerge.HasValue && mergeIdx <= endIdx)
            postMergeBody = RecoverStructure(cfg, mergeIdx, endIdx, loopHeaderOffset, loopExitOffsets, depth + 1);

        return new IRIfThenElse(
            condBlock.BodyInstructions,
            condBlock.Terminator ?? throw new InvalidOperationException("Condition block must have a terminator"),
            thenBranch,
            elseBranch,
            postMergeBody,
            PreConditionDepth: preCondDepth);
    }





    /// <summary>
    /// Build a switch node.
    /// </summary>
    private static StructuredIRNode BuildSwitch(
        ControlFlowGraph cfg, int switchBlockIndex,
        int? loopHeaderOffset = null, IReadOnlySet<int>? loopExitOffsets = null,
        int depth = 0)
    {
        var swBlock = cfg.Blocks[switchBlockIndex];
        if (swBlock.SwitchTargets.Count == 0)
            return new IRBlock(swBlock.BodyInstructions, swBlock.Terminator);

        var caseMap = new Dictionary<int, List<int>>();
        for (int i = 0; i < swBlock.SwitchTargets.Count; i++)
        {
            var target = swBlock.SwitchTargets[i];
            if (cfg.OffsetToBlockIndex.TryGetValue(target, out var targetIdx))
            {
                if (!caseMap.ContainsKey(targetIdx))
                    caseMap[targetIdx] = new List<int>();
                caseMap[targetIdx].Add(i);
            }
        }

        if (caseMap.Count == 0)
        {
            return new IRBlock(new List<AotCoreIrInstructionArtifact>(), null);
        }

        int maxTargetIdx = caseMap.Keys.Max();
        int mergeIdx = maxTargetIdx + 1;
        int endIdx = cfg.Blocks.Count - 1;

        int? originalSwitchMergeOffset = null;
        // The merge block follows the last switch target. When the switch has a
        // default case (e.g., `ldc.i4.0; stloc; br merge`), the block right after
        // maxTargetIdx is the default body, NOT the merge point. Follow the default
        // block's branch chain to find the actual merge offset — case body br/leave
        // instructions always target this final merge point.
        {
            int checkIdx = mergeIdx;
            while (checkIdx <= endIdx)
            {
                var checkBlock = cfg.Blocks[checkIdx];
                if (checkBlock.Terminator is { Op: "br" or "leave" })
                {
                    int targetOffset = GetRequiredIntOperand(checkBlock.Terminator);
                    if (cfg.OffsetToBlockIndex.TryGetValue(targetOffset, out var targetBlockIdx) && targetBlockIdx > checkIdx)
                    {
                        checkIdx = targetBlockIdx;
                        continue;
                    }
                }
                // Block has no branch-to-follow terminator. If it also has no
                // terminator at all (empty Op), it may be a fall-through block
                // whose next block is the real merge point — keep advancing.
                if (checkBlock.Terminator is null || string.IsNullOrEmpty(checkBlock.Terminator.Op))
                {
                    checkIdx++;
                    continue;
                }
                originalSwitchMergeOffset = checkBlock.StartOffset;
                break;
            }
        }

        // --- Build default body first ---
        StructuredIRNode? defaultBody = null;
        var coveredBlocks = new HashSet<int>(caseMap.Keys);
        var defaultBlocks = Enumerable.Range(switchBlockIndex + 1, mergeIdx - switchBlockIndex - 1)
            .Where(i => !coveredBlocks.Contains(i))
            .ToList();
        if (defaultBlocks.Count > 0)
        {
            defaultBody = RecoverStructure(cfg, defaultBlocks[0], defaultBlocks[^1], loopHeaderOffset, loopExitOffsets, depth + 1);

            // Handle default body redirect stubs BEFORE stripping trailing branches:
            // when the default body is just `br target` (empty body), and target is
            // a post-switch block, inline the target block into the default body.
            if (defaultBody is IRBlock { BodyInstructions.Count: 0, Terminator: { Op: "br" } } redirectStub)
            {
                int targetOffset = GetRequiredIntOperand(redirectStub.Terminator);
                if (cfg.OffsetToBlockIndex.TryGetValue(targetOffset, out var targetIdx) && targetIdx >= mergeIdx)
                {
                    var targetBlock = cfg.Blocks[targetIdx];
                    if (targetBlock.Terminator is { Op: "br" or "leave" })
                    {
                        defaultBody = new IRBlock(targetBlock.BodyInstructions, null);
                        mergeIdx = targetIdx + 1;
                    }
                    else
                    {
                        defaultBody = new IRBlock(targetBlock.BodyInstructions, targetBlock.Terminator);
                        mergeIdx = targetIdx;
                    }
                }
            }

            int? switchMergeOffset2 = mergeIdx <= endIdx ? cfg.Blocks[mergeIdx].StartOffset : null;
            if (switchMergeOffset2.HasValue)
                defaultBody = RemoveTrailingBranch(defaultBody, switchMergeOffset2.Value);
        }

        // --- Build case bodies with the (possibly updated) merge point ---
        // Case bodies are bounded by the original maxTargetIdx (not mergeIdx),
        // because mergeIdx may have advanced due to redirect-stub inlining
        // but the case body ranges must not include post-switch blocks.
        var caseBodies = new Dictionary<int, StructuredIRNode>();
        var fallthroughCaseValues = new HashSet<int>();
        var sortedCaseKeys = caseMap.Keys.OrderBy(k => k).ToList();
        for (int ci = 0; ci < sortedCaseKeys.Count; ci++)
        {
            int caseBlockIdx = sortedCaseKeys[ci];
            int nextCaseStart = (ci < sortedCaseKeys.Count - 1)
                ? sortedCaseKeys[ci + 1]
                : endIdx + 1;
            int bodyEnd = Math.Min(nextCaseStart - 1, maxTargetIdx);
            StructuredIRNode body;
            if (caseBlockIdx <= bodyEnd)
                body = RecoverStructure(cfg, caseBlockIdx, bodyEnd, loopHeaderOffset, loopExitOffsets, depth + 1);
            else
                body = new IRSequence(Array.Empty<StructuredIRNode>());

            bool hadTrailingBranch = false;
            if (originalSwitchMergeOffset.HasValue)
            {
                var beforeBody = body;
                body = RemoveTrailingBranch(body, originalSwitchMergeOffset.Value);
                hadTrailingBranch = !ReferenceEquals(beforeBody, body);
            }

            // Detect fallthrough: a case body that has no control flow terminator
            // AND no trailing branch was stripped (i.e., the body genuinely falls through).
            if (!IsControlFlowTerminator(body) && !hadTrailingBranch && ci < sortedCaseKeys.Count - 1)
            {
                foreach (var caseValue in caseMap[caseBlockIdx])
                    fallthroughCaseValues.Add(caseValue);
            }

            foreach (var caseValue in caseMap[caseBlockIdx])
                caseBodies[caseValue] = body;
        }

        int exitOffset = mergeIdx <= endIdx ? cfg.Blocks[mergeIdx].StartOffset : -1;
        return new IRSwitch(swBlock.BodyInstructions, caseBodies, defaultBody, exitOffset, fallthroughCaseValues);
    }





    // ────────────────────────────────────────────────────────────────────────────
    // Node Splitting — make irreducible CFGs reducible
    // ────────────────────────────────────────────────────────────────────────────

    /// <summary>
    /// Attempt to make an irreducible CFG reducible by node splitting.
    /// For each block that receives backward edges without being a proper loop
    /// header, create a copy and redirect all forward (external) predecessors
    /// to the copy, so the original block becomes a single-entry loop header.
    /// </summary>
    private static ControlFlowGraph ApplyNodeSplitting(ControlFlowGraph cfg)
    {
        var blocks = cfg.Blocks;
        var offsetToBlockIndex = cfg.OffsetToBlockIndex;
        var loopHeaders = cfg.LoopHeaders;
        var preds = ComputePredecessors(blocks);

        // Pass 1: find split candidates — blocks targeted by unnatural backward branches
        var splitTargets = FindUnnaturalBackedgeTargets(blocks, offsetToBlockIndex, loopHeaders);

        if (splitTargets.Count == 0)
            return cfg; // nothing to split

        int nextSynthOffset = -1;
        var newBlocks = new List<BasicBlock>();
        var newOffsetMap = new Dictionary<int, int>();

        // Maps original index → new index of the split copy (for redirection)
        var splitCopyIndex = new Dictionary<int, int>();

        // Helper: check if an edge from sourceIdx to targetOffset in the original CFG is forward
        bool IsForwardEdge(int sourceIdx, int? targetOffset)
        {
            if (!targetOffset.HasValue) return false;
            if (!offsetToBlockIndex.TryGetValue(targetOffset.Value, out var targetIdx)) return false;
            return sourceIdx < targetIdx;
        }

        // Pass 1: build newBlocks with clones for split targets.
        // Clones get a synthetic negative offset and are inserted BEFORE the original block.
        // The clone inherits the original's outgoing targets; forward edges from non-latch
        // predecessors will be redirected to the clone in pass 2.
        for (int i = 0; i < blocks.Count; i++)
        {
            if (!splitTargets.Contains(i))
            {
                newOffsetMap[blocks[i].StartOffset] = newBlocks.Count;
                newBlocks.Add(blocks[i]);
                continue;
            }

            var original = blocks[i];
            int synthOffset = nextSynthOffset--;

            // Create a clone of the block with the same body + terminator + targets.
            // It bridges external (forward) predecessors into the loop header.
            var clone = new BasicBlock(
                synthOffset,
                original.BodyInstructions,
                original.Terminator,
                BasicBlockKind.Normal,
                original.BranchTarget,
                original.ConditionalTarget,
                original.SwitchTargets,
                original.IsTerminal);

            int cloneIdx = newBlocks.Count;
            splitCopyIndex[i] = cloneIdx;
            newOffsetMap[synthOffset] = cloneIdx;
            newBlocks.Add(clone);

            // Original block follows — now only reached from backward edges
            newOffsetMap[original.StartOffset] = newBlocks.Count;
            newBlocks.Add(original);
        }

        // Pass 2: redirect forward edges from non-latch predecessors to the split copies.
        // Iterate original indices; for each block, check if its outgoing targets
        // point to a split target via a forward edge. If so, redirect to the clone.
        for (int i = 0; i < blocks.Count; i++)
        {
            int newIdx = newOffsetMap[blocks[i].StartOffset];
            var b = newBlocks[newIdx];

            int? newBranchTarget = b.BranchTarget;
            if (IsForwardEdge(i, b.BranchTarget) && splitTargets.Contains(offsetToBlockIndex[b.BranchTarget!.Value]))
                newBranchTarget = FindCloneOffset(offsetToBlockIndex[b.BranchTarget.Value], splitCopyIndex, newOffsetMap);

            int? newConditionalTarget = b.ConditionalTarget;
            if (IsForwardEdge(i, b.ConditionalTarget) && splitTargets.Contains(offsetToBlockIndex[b.ConditionalTarget!.Value]))
                newConditionalTarget = FindCloneOffset(offsetToBlockIndex[b.ConditionalTarget.Value], splitCopyIndex, newOffsetMap);

            IReadOnlyList<int> newSwitchTargets = b.SwitchTargets;
            if (b.SwitchTargets.Count > 0)
            {
                var switched = false;
                var list = new List<int>(b.SwitchTargets.Count);
                foreach (var st in b.SwitchTargets)
                {
                    if (offsetToBlockIndex.TryGetValue(st, out var stIdx) &&
                        IsForwardEdge(i, st) && splitTargets.Contains(stIdx))
                    {
                        var cloneOff = FindCloneOffset(stIdx, splitCopyIndex, newOffsetMap);
                        list.Add(cloneOff ?? st);
                        if (cloneOff != st) switched = true;
                    }
                    else
                    {
                        list.Add(st);
                    }
                }
                if (switched) newSwitchTargets = list;
            }

            if (newBranchTarget != b.BranchTarget ||
                newConditionalTarget != b.ConditionalTarget ||
                newSwitchTargets != b.SwitchTargets)
            {
                newBlocks[newIdx] = new BasicBlock(
                    b.StartOffset, b.BodyInstructions, b.Terminator, b.Kind,
                    newBranchTarget, newConditionalTarget, newSwitchTargets, b.IsTerminal);
            }
        }

        // Pass 3: Recompute dominators + loop headers + reducibility
        var newDominators = ComputeDominators(newBlocks);
        var newLoopHeaders = FindLoopHeaders(newBlocks, newOffsetMap, newDominators);
        bool reducible = IsCfgReducible(newBlocks, newOffsetMap, newLoopHeaders);

        return new ControlFlowGraph(newBlocks, newOffsetMap, newLoopHeaders, reducible);
    }





    /// <summary>
    /// Find blocks that are the target of at least one unnatural backward branch
    /// (backward edge where the target is not a loop header, or the source is not a latch).
    /// </summary>
    private static HashSet<int> FindUnnaturalBackedgeTargets(
        IReadOnlyList<BasicBlock> blocks,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        IReadOnlyDictionary<int, NaturalLoopInfo> loopHeaders)
    {
        var targets = new HashSet<int>();
        for (int i = 0; i < blocks.Count; i++)
        {
            var block = blocks[i];
            CheckBackwardEdge(block.ConditionalTarget, i, offsetToBlockIndex, loopHeaders, targets);
            CheckBackwardEdge(block.BranchTarget, i, offsetToBlockIndex, loopHeaders, targets);
            if (block.SwitchTargets.Count > 0)
            {
                foreach (var t in block.SwitchTargets)
                    CheckBackwardEdge(t, i, offsetToBlockIndex, loopHeaders, targets);
            }
        }
        return targets;
    }





    private static void CheckBackwardEdge(
        int? targetOffset, int sourceIdx,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        IReadOnlyDictionary<int, NaturalLoopInfo> loopHeaders,
        HashSet<int> targets)
    {
        if (!targetOffset.HasValue) return;
        if (!offsetToBlockIndex.TryGetValue(targetOffset.Value, out var targetIdx)) return;
        if (targetIdx >= sourceIdx) return; // not a backward edge

        // Unnatural if target is not a loop header, or source is not a latch
        if (!loopHeaders.ContainsKey(targetIdx) ||
            !loopHeaders[targetIdx].LatchIndices.Contains(sourceIdx))
        {
            targets.Add(targetIdx);
        }
    }

}
