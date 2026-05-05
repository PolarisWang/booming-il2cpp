param([string]$FilePath = "src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.StructuredControlFlow.cs")

function Normalize-LineEndings($text) {
    return $text -replace "`r`n", "`n"
}

$content = Get-Content $FilePath -Raw
$content = Normalize-LineEndings($content)

# ============================================================
# 1. Remove old StructuredNode type definitions (keep NaturalLoopInfo)
# ============================================================
# Match from "// Structured node tree" up to (but not including) "private sealed record NaturalLoopInfo"
$old_types_start = Normalize-LineEndings(@"
    // Structured node tree
    private abstract record StructuredNode;
    private sealed record BasicBlockNode(BasicBlock Block) : StructuredNode;
    private sealed record SequenceNode(IReadOnlyList<StructuredNode> Nodes) : StructuredNode;
    private sealed record IfThenElseNode(
        BasicBlock ConditionBlock,
        StructuredNode ThenBranch,
        StructuredNode? ElseBranch,
        int? MergeOffset) : StructuredNode;
    private sealed record SwitchNode(
        BasicBlock SwitchBlock,
        IReadOnlyDictionary<int, StructuredNode> CaseBodies,
        StructuredNode? DefaultBody,
        int? MergeOffset) : StructuredNode;

    private sealed record LoopNode(
        BasicBlock HeaderBlock,
        StructuredNode Body,
        int? ExitOffset,
        bool IsWhile,
        BasicBlock? LatchBlock) : StructuredNode;

"@)

$content = $content.Replace($old_types_start, "")

# ============================================================
# 2. Remove LoopContextStack (no longer needed)
# ============================================================
$loopContextLine = Normalize-LineEndings("    // Break/continue context stack
    private static readonly Stack<(int headerOffset, int exitOffset)> LoopContextStack = new();

")
$content = $content.Replace($loopContextLine, "")

# ============================================================
# 3. Replace old recovery methods with new StructuredIR versions
# ============================================================
$old_recovery_header = Normalize-LineEndings(@"
    // ──────────────────────────────────────────────
    // Structured Control Flow Recovery
    // ──────────────────────────────────────────────

    /// <summary>
    /// Recover structured control flow from the CFG.
    /// </summary>
    private static StructuredNode RecoverStructure(
        ControlFlowGraph cfg,
        int startIndex, int endIndex)
"@)

$new_recovery_header = Normalize-LineEndings(@"
    // ──────────────────────────────────────────────
    // Structured Control Flow Recovery → StructuredIR
    // ──────────────────────────────────────────────

    /// <summary>
    /// Recover structured control flow from the CFG as a pure StructuredIR tree.
    /// </summary>
    private static StructuredIRNode RecoverStructure(
        ControlFlowGraph cfg,
        int startIndex, int endIndex,
        int? loopHeaderOffset = null,
        int? loopExitOffset = null)
"@)

$content = $content.Replace($old_recovery_header, $new_recovery_header)

# ============================================================
# 4. Replace BuildLoop signature + body
# ============================================================
$old_buildloop_start = Normalize-LineEndings(@"
    /// <summary>
    /// Build a loop node from a natural loop.
    /// </summary>
    private static StructuredNode BuildLoop(
        ControlFlowGraph cfg,
        int headerIndex, int endIndex,
        NaturalLoopInfo loopInfo)
    {
        var header = cfg.Blocks[headerIndex];
        int bodyStart = headerIndex + 1;

        int maxLatchIdx = loopInfo.LatchIndices.Max();
        int bodyEnd = maxLatchIdx;
        if (bodyEnd > endIndex) bodyEnd = endIndex;

        int exitIdx = bodyEnd + 1;
        while (exitIdx <= endIndex && loopInfo.BodyIndices.Contains(exitIdx))
            exitIdx++;

        int? exitOffset = exitIdx <= endIndex ? cfg.Blocks[exitIdx].StartOffset : null;

        bool isWhile;
        BasicBlock? latchBlock = null;

        if (header.ConditionalTarget.HasValue && IsConditionalBranchOpcode(header.Terminator?.Op ?? ""))
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

        StructuredNode body;
        if (bodyStart <= bodyEnd)
            body = RecoverStructure(cfg, bodyStart, bodyEnd);
        else
            body = new SequenceNode(Array.Empty<StructuredNode>());

        return new LoopNode(header, body, exitOffset, isWhile, latchBlock);
    }
"@)

$new_buildloop = Normalize-LineEndings(@"
    /// <summary>
    /// Build a loop node from a natural loop.
    /// </summary>
    private static StructuredIRNode BuildLoop(
        ControlFlowGraph cfg,
        int headerIndex, int endIndex,
        NaturalLoopInfo loopInfo,
        int? outerLoopHeaderOffset = null,
        int? outerLoopExitOffset = null)
    {
        var header = cfg.Blocks[headerIndex];
        int bodyStart = headerIndex + 1;

        int maxLatchIdx = loopInfo.LatchIndices.Max();
        int bodyEnd = maxLatchIdx;
        if (bodyEnd > endIndex) bodyEnd = endIndex;

        int exitIdx = bodyEnd + 1;
        while (exitIdx <= endIndex && loopInfo.BodyIndices.Contains(exitIdx))
            exitIdx++;

        int? exitOffset = exitIdx <= endIndex ? cfg.Blocks[exitIdx].StartOffset : null;
        int headerOffset = header.StartOffset;

        bool isWhile;
        BasicBlock? latchBlock = null;

        if (header.ConditionalTarget.HasValue && IsConditionalBranchOpcode(header.Terminator?.Op ?? ""))
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

        // Build body with the loop's own context for break/continue detection
        StructuredIRNode body;
        if (bodyStart <= bodyEnd)
            body = RecoverStructure(cfg, bodyStart, bodyEnd, headerOffset, exitOffset);
        else
            body = new IRSequence(Array.Empty<StructuredIRNode>());

        int exitVal = exitOffset ?? -1;

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
                headerOffset,
                exitVal);
        }
    }
"@)

$content = $content.Replace($old_buildloop_start, $new_buildloop)

# ============================================================
# 5. Replace BuildIfThenElse signature + body
# ============================================================
$old_buildite_start = Normalize-LineEndings(@"
    /// <summary>
    /// Build an if-then-else node starting from a conditional branch block.
    /// </summary>
    private static StructuredNode BuildIfThenElse(ControlFlowGraph cfg, int conditionBlockIndex)
    {
        var condBlock = cfg.Blocks[conditionBlockIndex];
        if (!condBlock.ConditionalTarget.HasValue)
            return new BasicBlockNode(condBlock);

        var trueTarget = condBlock.ConditionalTarget.Value;
        if (!cfg.OffsetToBlockIndex.TryGetValue(trueTarget, out var trueBlockIdx))
            return new BasicBlockNode(condBlock);

        int falseBlockIdx = conditionBlockIndex + 1;
        int endIdx = cfg.Blocks.Count - 1;

        int? mergeOffset = FindMergePoint(cfg, conditionBlockIndex, trueBlockIdx, falseBlockIdx);

        StructuredNode thenBranch;
        if (mergeOffset.HasValue && cfg.OffsetToBlockIndex.TryGetValue(mergeOffset.Value, out var mergeIdx))
        {
            thenBranch = RecoverStructure(cfg, trueBlockIdx, mergeIdx - 1);
            if (falseBlockIdx < mergeIdx)
            {
                var elseBranch = RecoverStructure(cfg, falseBlockIdx, mergeIdx - 1);
                return new IfThenElseNode(condBlock, thenBranch, elseBranch, mergeOffset);
            }
            else
            {
                return new IfThenElseNode(condBlock, thenBranch, null, mergeOffset);
            }
        }
        else
        {
            thenBranch = RecoverStructure(cfg, trueBlockIdx, endIdx);
            if (falseBlockIdx <= endIdx)
            {
                var elseBranch = RecoverStructure(cfg, falseBlockIdx, endIdx);
                return new IfThenElseNode(condBlock, thenBranch, elseBranch, null);
            }
            return new IfThenElseNode(condBlock, thenBranch, null, null);
        }
    }
"@)

$new_buildite = Normalize-LineEndings(@"
    /// <summary>
    /// Build an if-then-else node starting from a conditional branch block.
    /// </summary>
    private static StructuredIRNode BuildIfThenElse(
        ControlFlowGraph cfg, int conditionBlockIndex,
        int? loopHeaderOffset = null, int? loopExitOffset = null)
    {
        var condBlock = cfg.Blocks[conditionBlockIndex];
        if (!condBlock.ConditionalTarget.HasValue)
            return new IRBlock(condBlock.BodyInstructions, condBlock.Terminator);

        var trueTarget = condBlock.ConditionalTarget.Value;
        if (!cfg.OffsetToBlockIndex.TryGetValue(trueTarget, out var trueBlockIdx))
            return new IRBlock(condBlock.BodyInstructions, condBlock.Terminator);

        int falseBlockIdx = conditionBlockIndex + 1;
        int endIdx = cfg.Blocks.Count - 1;

        int? mergeOffset = FindMergePoint(cfg, conditionBlockIndex, trueBlockIdx, falseBlockIdx);

        StructuredIRNode thenBranch;
        StructuredIRNode? elseBranch;
        int? effectiveMerge = null;

        if (mergeOffset.HasValue && cfg.OffsetToBlockIndex.TryGetValue(mergeOffset.Value, out var mergeIdx))
        {
            thenBranch = RecoverStructure(cfg, trueBlockIdx, mergeIdx - 1, loopHeaderOffset, loopExitOffset);
            if (falseBlockIdx < mergeIdx)
            {
                elseBranch = RecoverStructure(cfg, falseBlockIdx, mergeIdx - 1, loopHeaderOffset, loopExitOffset);
            }
            else
            {
                elseBranch = null;
            }
            effectiveMerge = mergeOffset;
        }
        else
        {
            thenBranch = RecoverStructure(cfg, trueBlockIdx, endIdx, loopHeaderOffset, loopExitOffset);
            if (falseBlockIdx <= endIdx)
                elseBranch = RecoverStructure(cfg, falseBlockIdx, endIdx, loopHeaderOffset, loopExitOffset);
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

        return new IRIfThenElse(
            condBlock.BodyInstructions,
            condBlock.Terminator ?? throw new InvalidOperationException("Condition block must have a terminator"),
            thenBranch,
            elseBranch);
    }
"@)

$content = $content.Replace($old_buildite_start, $new_buildite)

# ============================================================
# 6. Replace BuildSwitch signature + body
# ============================================================
$old_buildsw_start = Normalize-LineEndings(@"
    /// <summary>
    /// Build a switch node.
    /// </summary>
    private static StructuredNode BuildSwitch(ControlFlowGraph cfg, int switchBlockIndex)
    {
        var swBlock = cfg.Blocks[switchBlockIndex];
        if (swBlock.SwitchTargets.Count == 0)
            return new BasicBlockNode(swBlock);

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

        int maxTargetIdx = caseMap.Keys.Max();
        int mergeIdx = maxTargetIdx + 1;
        int endIdx = cfg.Blocks.Count - 1;

        var caseBodies = new Dictionary<int, StructuredNode>();
        foreach (var kvp in caseMap)
        {
            int nextCaseStart = caseMap.Keys.Where(k => k > kvp.Key).DefaultIfEmpty(endIdx + 1).Min();
            int bodyEnd = Math.Min(nextCaseStart - 1, mergeIdx - 1);
            StructuredNode body;
            if (kvp.Key <= bodyEnd)
                body = RecoverStructure(cfg, kvp.Key, bodyEnd);
            else
                body = new SequenceNode(Array.Empty<StructuredNode>());

            foreach (var caseValue in kvp.Value)
                caseBodies[caseValue] = body;
        }

        StructuredNode? defaultBody = null;
        var coveredBlocks = new HashSet<int>(caseMap.Keys);
        var defaultBlocks = Enumerable.Range(switchBlockIndex + 1, mergeIdx - switchBlockIndex - 1)
            .Where(i => !coveredBlocks.Contains(i))
            .ToList();
        if (defaultBlocks.Count > 0)
            defaultBody = RecoverStructure(cfg, defaultBlocks[0], defaultBlocks[^1]);

        int? mergeOffset = mergeIdx <= endIdx ? cfg.Blocks[mergeIdx].StartOffset : null;
        return new SwitchNode(swBlock, caseBodies, defaultBody, mergeOffset);
    }
"@)

$new_buildsw = Normalize-LineEndings(@"
    /// <summary>
    /// Build a switch node.
    /// </summary>
    private static StructuredIRNode BuildSwitch(
        ControlFlowGraph cfg, int switchBlockIndex,
        int? loopHeaderOffset = null, int? loopExitOffset = null)
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

        int maxTargetIdx = caseMap.Keys.Max();
        int mergeIdx = maxTargetIdx + 1;
        int endIdx = cfg.Blocks.Count - 1;

        var caseBodies = new Dictionary<int, StructuredIRNode>();
        foreach (var kvp in caseMap)
        {
            int nextCaseStart = caseMap.Keys.Where(k => k > kvp.Key).DefaultIfEmpty(endIdx + 1).Min();
            int bodyEnd = Math.Min(nextCaseStart - 1, mergeIdx - 1);
            StructuredIRNode body;
            if (kvp.Key <= bodyEnd)
                body = RecoverStructure(cfg, kvp.Key, bodyEnd, loopHeaderOffset, loopExitOffset);
            else
                body = new IRSequence(Array.Empty<StructuredIRNode>());

            // Eliminate trailing branches that target the switch merge point
            int? switchMergeOffset = mergeIdx <= endIdx ? cfg.Blocks[mergeIdx].StartOffset : null;
            if (switchMergeOffset.HasValue)
                body = RemoveTrailingBranch(body, switchMergeOffset.Value);

            foreach (var caseValue in kvp.Value)
                caseBodies[caseValue] = body;
        }

        StructuredIRNode? defaultBody = null;
        var coveredBlocks = new HashSet<int>(caseMap.Keys);
        var defaultBlocks = Enumerable.Range(switchBlockIndex + 1, mergeIdx - switchBlockIndex - 1)
            .Where(i => !coveredBlocks.Contains(i))
            .ToList();
        if (defaultBlocks.Count > 0)
        {
            defaultBody = RecoverStructure(cfg, defaultBlocks[0], defaultBlocks[^1], loopHeaderOffset, loopExitOffset);
            int? switchMergeOffset2 = mergeIdx <= endIdx ? cfg.Blocks[mergeIdx].StartOffset : null;
            if (switchMergeOffset2.HasValue)
                defaultBody = RemoveTrailingBranch(defaultBody, switchMergeOffset2.Value);
        }

        int exitOffset = mergeIdx <= endIdx ? cfg.Blocks[mergeIdx].StartOffset : -1;
        return new IRSwitch(swBlock.BodyInstructions, caseBodies, defaultBody, exitOffset);
    }
"@)

$content = $content.Replace($old_buildsw_start, $new_buildsw)

# ============================================================
# 7. Add RemoveTrailingBranch helper (insert before RecoverStructure)
# ============================================================
$remove_helper = Normalize-LineEndings(@"

    /// <summary>
    /// Walk a StructuredIR tree and remove trailing br/leave that targets a specific offset.
    /// The branch is redundant because structured control flow provides implicit fallthrough.
    /// </summary>
    private static StructuredIRNode RemoveTrailingBranch(StructuredIRNode node, int targetOffset)
    {
        if (node is IRBlock block && block.Terminator != null &&
            (block.Terminator.Op == ""br"" || block.Terminator.Op == ""leave"") &&
            GetRequiredIntOperand(block.Terminator) == targetOffset)
        {
            return new IRBlock(block.BodyInstructions, null);
        }

        if (node is IRSequence seq && seq.Nodes.Count > 0)
        {
            var trimmed = RemoveTrailingBranch(seq.Nodes[^1], targetOffset);
            if (!ReferenceEquals(trimmed, seq.Nodes[^1]))
            {
                var newNodes = new List<StructuredIRNode>(seq.Nodes);
                newNodes[^1] = trimmed;
                return new IRSequence(newNodes);
            }
        }

        return node;
    }
"@)

# Insert RemoveTrailingBranch after the last CFG helper (FindMergePoint's closing })
$findmerge_end = Normalize-LineEndings(@"
        return null;
    }
"@)

# Find the last occurrence (which is the end of FindMergePoint) and insert after it
$lastPos = $content.LastIndexOf($findmerge_end)
if ($lastPos -ge 0) {
    $insertPos = $lastPos + $findmerge_end.Length
    $content = $content.Substring(0, $insertPos) + $remove_helper + $content.Substring($insertPos)
}

# ============================================================
# 8. Also update the RecoverStructure body (it was partially changed by the header replacement)
#    The old body follows the new header, so we need to replace it completely.
# ============================================================
$old_recover_body = Normalize-LineEndings(@"
    {
        if (startIndex > endIndex)
            return new SequenceNode(Array.Empty<StructuredNode>());

        if (startIndex == endIndex)
        {
            var block = cfg.Blocks[startIndex];
            if (block.Terminator == null || block.IsTerminal || block.Terminator.Op is ""br"" or ""leave"")
            {
                return new BasicBlockNode(block);
            }
            if (IsConditionalBranchOpcode(block.Terminator.Op))
            {
                return BuildIfThenElse(cfg, startIndex);
            }
            if (IsSwitchOpcode(block.Terminator.Op))
            {
                return BuildSwitch(cfg, startIndex);
            }
            return new BasicBlockNode(block);
        }

        // Check if startIndex is a loop header
        if (cfg.LoopHeaders.TryGetValue(startIndex, out var loopInfo))
        {
            return BuildLoop(cfg, startIndex, endIndex, loopInfo);
        }

        // Try to find a conditional branch at the start of the interval
        var firstBlock = cfg.Blocks[startIndex];
        if (firstBlock.ConditionalTarget.HasValue && IsConditionalBranchOpcode(firstBlock.Terminator?.Op ?? """"))
        {
            var ite = BuildIfThenElse(cfg, startIndex);
            if (ite is IfThenElseNode iteNode)
            {
                int afterMergeIdx;
                if (iteNode.MergeOffset.HasValue &&
                    cfg.OffsetToBlockIndex.TryGetValue(iteNode.MergeOffset.Value, out var mergeIdx))
                {
                    afterMergeIdx = mergeIdx + 1;
                }
                else
                {
                    afterMergeIdx = endIndex + 1;
                }

                var nodes = new List<StructuredNode> { iteNode };
                if (afterMergeIdx <= endIndex)
                {
                    nodes.Add(RecoverStructure(cfg, afterMergeIdx, endIndex));
                }
                return new SequenceNode(nodes);
            }
        }

        // Try to find a switch at the start
        if (IsSwitchOpcode(firstBlock.Terminator?.Op ?? """"))
        {
            var sw = BuildSwitch(cfg, startIndex);
            if (sw is SwitchNode swNode)
            {
                int afterMergeIdx;
                if (swNode.MergeOffset.HasValue &&
                    cfg.OffsetToBlockIndex.TryGetValue(swNode.MergeOffset.Value, out var mergeIdx))
                {
                    afterMergeIdx = mergeIdx + 1;
                }
                else
                {
                    afterMergeIdx = endIndex + 1;
                }

                var nodes = new List<StructuredNode> { swNode };
                if (afterMergeIdx <= endIndex)
                {
                    nodes.Add(RecoverStructure(cfg, afterMergeIdx, endIndex));
                }
                return new SequenceNode(nodes);
            }
        }

        // Otherwise: sequential blocks
        var seqNodes = new List<StructuredNode>();
        int idx = startIndex;
        while (idx <= endIndex)
        {
            var block = cfg.Blocks[idx];

            if (cfg.LoopHeaders.TryGetValue(idx, out var currentLoop))
            {
                var loop = BuildLoop(cfg, idx, endIndex, currentLoop);
                seqNodes.Add(loop);
                int nextIdx = idx + 1;
                while (nextIdx <= endIndex && currentLoop.BodyIndices.Contains(nextIdx))
                    nextIdx++;
                idx = nextIdx;
            }
            else if (block.ConditionalTarget.HasValue && IsConditionalBranchOpcode(block.Terminator?.Op ?? """"))
            {
                var ite = BuildIfThenElse(cfg, idx);
                seqNodes.Add(ite);
                if (ite is IfThenElseNode iteResult && iteResult.MergeOffset.HasValue &&
                    cfg.OffsetToBlockIndex.TryGetValue(iteResult.MergeOffset.Value, out var mergeIdx))
                {
                    idx = mergeIdx + 1;
                }
                else
                {
                    idx = endIndex + 1;
                }
            }
            else if (IsSwitchOpcode(block.Terminator?.Op ?? """"))
            {
                var sw = BuildSwitch(cfg, idx);
                seqNodes.Add(sw);
                if (sw is SwitchNode swResult && swResult.MergeOffset.HasValue &&
                    cfg.OffsetToBlockIndex.TryGetValue(swResult.MergeOffset.Value, out var swMergeIdx))
                {
                    idx = swMergeIdx + 1;
                }
                else
                {
                    idx = endIndex + 1;
                }
            }
            else
            {
                seqNodes.Add(new BasicBlockNode(block));
                idx++;
            }
        }

        return seqNodes.Count == 1 ? seqNodes[0] : new SequenceNode(seqNodes);
    }
"@)

$new_recover_body = Normalize-LineEndings(@"
    {
        if (startIndex > endIndex)
            return new IRSequence(Array.Empty<StructuredIRNode>());

        if (startIndex == endIndex)
        {
            var block = cfg.Blocks[startIndex];

            // Check for break/continue targeting the surrounding loop context
            if (block.Terminator != null && (block.Terminator.Op is ""br"" or ""leave""))
            {
                int target = GetRequiredIntOperand(block.Terminator);
                if (loopExitOffset.HasValue && target == loopExitOffset.Value)
                    return new IRBreak;
                if (loopHeaderOffset.HasValue && target == loopHeaderOffset.Value)
                    return new IRContinue;
            }

            if (block.Terminator == null || block.IsTerminal || block.Terminator.Op is ""br"" or ""leave"")
            {
                return new IRBlock(block.BodyInstructions, block.Terminator);
            }
            if (IsConditionalBranchOpcode(block.Terminator.Op))
            {
                return BuildIfThenElse(cfg, startIndex, loopHeaderOffset, loopExitOffset);
            }
            if (IsSwitchOpcode(block.Terminator.Op))
            {
                return BuildSwitch(cfg, startIndex, loopHeaderOffset, loopExitOffset);
            }
            return new IRBlock(block.BodyInstructions, block.Terminator);
        }

        // Check if startIndex is a loop header
        if (cfg.LoopHeaders.TryGetValue(startIndex, out var loopInfo))
        {
            return BuildLoop(cfg, startIndex, endIndex, loopInfo, loopHeaderOffset, loopExitOffset);
        }

        // Try to find a conditional branch at the start of the interval
        var firstBlock = cfg.Blocks[startIndex];
        if (firstBlock.ConditionalTarget.HasValue && IsConditionalBranchOpcode(firstBlock.Terminator?.Op ?? """"))
        {
            var ite = BuildIfThenElse(cfg, startIndex, loopHeaderOffset, loopExitOffset);
            if (ite is IRIfThenElse)
            {
                // Determine merge point from CFG
                int trueBlockIdx = cfg.OffsetToBlockIndex[firstBlock.ConditionalTarget.Value];
                int falseBlockIdx = startIndex + 1;
                int? mergeOffset = FindMergePoint(cfg, startIndex, trueBlockIdx, falseBlockIdx);

                int afterMergeIdx;
                if (mergeOffset.HasValue &&
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
                    nodes.Add(RecoverStructure(cfg, afterMergeIdx, endIndex, loopHeaderOffset, loopExitOffset));
                }
                return new IRSequence(nodes);
            }
        }

        // Try to find a switch at the start
        if (IsSwitchOpcode(firstBlock.Terminator?.Op ?? """"))
        {
            var sw = BuildSwitch(cfg, startIndex, loopHeaderOffset, loopExitOffset);
            if (sw is IRSwitch swNode)
            {
                // Determine switch merge point: after the last target block
                int maxTargetIdx = startIndex;
                foreach (var t in firstBlock.SwitchTargets)
                {
                    if (cfg.OffsetToBlockIndex.TryGetValue(t, out var tIdx) && tIdx > maxTargetIdx)
                        maxTargetIdx = tIdx;
                }
                int mergeIdx = maxTargetIdx + 1;

                int afterMergeIdx = mergeIdx <= endIndex ? mergeIdx : endIndex + 1;

                var nodes = new List<StructuredIRNode> { swNode };
                if (afterMergeIdx <= endIndex)
                {
                    nodes.Add(RecoverStructure(cfg, afterMergeIdx, endIndex, loopHeaderOffset, loopExitOffset));
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
                var loop = BuildLoop(cfg, idx, endIndex, currentLoop, loopHeaderOffset, loopExitOffset);
                seqNodes.Add(loop);
                int nextIdx = idx + 1;
                while (nextIdx <= endIndex && currentLoop.BodyIndices.Contains(nextIdx))
                    nextIdx++;
                idx = nextIdx;
            }
            else if (block.ConditionalTarget.HasValue && IsConditionalBranchOpcode(block.Terminator?.Op ?? """"))
            {
                var ite = BuildIfThenElse(cfg, idx, loopHeaderOffset, loopExitOffset);
                seqNodes.Add(ite);
                if (ite is IRIfThenElse)
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
            else if (IsSwitchOpcode(block.Terminator?.Op ?? """"))
            {
                var sw = BuildSwitch(cfg, idx, loopHeaderOffset, loopExitOffset);
                seqNodes.Add(sw);
                if (sw is IRSwitch)
                {
                    int maxTgt = idx;
                    foreach (var t in block.SwitchTargets)
                    {
                        if (cfg.OffsetToBlockIndex.TryGetValue(t, out var tIdx) && tIdx > maxTgt)
                            maxTgt = tIdx;
                    }
                    int swMergeIdx = maxTgt + 1;
                    idx = swMergeIdx <= endIndex ? swMergeIdx : endIndex + 1;
                }
                else
                {
                    idx = endIndex + 1;
                }
            }
            else
            {
                // Check for break/continue in sequential block
                var seqBlock = block;
                if (seqBlock.Terminator != null && (seqBlock.Terminator.Op is ""br"" or ""leave""))
                {
                    int seqTarget = GetRequiredIntOperand(seqBlock.Terminator);
                    if (loopExitOffset.HasValue && seqTarget == loopExitOffset.Value)
                    {
                        seqNodes.Add(new IRBreak);
                        idx++;
                        continue;
                    }
                    if (loopHeaderOffset.HasValue && seqTarget == loopHeaderOffset.Value)
                    {
                        seqNodes.Add(new IRContinue);
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
"@)

$content = $content.Replace($old_recover_body, $new_recover_body)

Set-Content $FilePath $content -NoNewLine

Write-Output "Rewrite complete. Verify the file builds correctly."
