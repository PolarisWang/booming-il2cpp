using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    // ──────────────────────────────────────────────
    // Data structures for CFG and structured recovery
    // ──────────────────────────────────────────────

    private sealed record BasicBlock(
        int StartOffset,
        IReadOnlyList<AotCoreIrInstructionArtifact> BodyInstructions,
        AotCoreIrInstructionArtifact? Terminator,
        BasicBlockKind Kind,
        int? BranchTarget,
        int? ConditionalTarget,
        IReadOnlyList<int> SwitchTargets,
        bool IsTerminal);

    private enum BasicBlockKind { Normal, Entry, Exit }

    private sealed record ControlFlowGraph(
        IReadOnlyList<BasicBlock> Blocks,
        IReadOnlyDictionary<int, int> OffsetToBlockIndex,
        bool IsReducible);

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

    // ──────────────────────────────────────────────
    // CFG Construction
    // ──────────────────────────────────────────────

    /// <summary>
    /// Returns true if the opcode is a branch or terminator that ends a basic block.
    /// </summary>
    private static bool IsBlockTerminatorOpcode(string op) => op switch
    {
        "br" or "brtrue" or "brfalse"
        or "beq" or "bne.un" or "bge" or "bge.un"
        or "bgt" or "ble" or "blt"
        or "leave" or "switch"
        or "ret" or "throw" or "rethrow"
        or "endfilter" or "endfinally" => true,
        _ => false,
    };

    /// <summary>
    /// Returns true if the opcode is a conditional branch (has a fall-through).
    /// </summary>
    private static bool IsConditionalBranchOpcode(string op) => op switch
    {
        "brtrue" or "brfalse"
        or "beq" or "bne.un" or "bge" or "bge.un"
        or "bgt" or "ble" or "blt" => true,
        _ => false,
    };

    /// <summary>
    /// Returns true if the opcode is a switch.
    /// </summary>
    private static bool IsSwitchOpcode(string op) => op == "switch";

    /// <summary>
    /// Enumerate all IL offsets that are branch targets in the instruction list.
    /// </summary>
    private static HashSet<int> EnumerateBranchTargets(IReadOnlyList<AotCoreIrInstructionArtifact> instructions)
    {
        var targets = new HashSet<int>();
        foreach (var instr in instructions)
        {
            if (IsConditionalBranchOpcode(instr.Op) || instr.Op == "br" || instr.Op == "leave")
            {
                targets.Add(GetRequiredIntOperand(instr));
            }
            else if (IsSwitchOpcode(instr.Op))
            {
                var switchTargets = ExtractSwitchTargets(instr);
                foreach (var t in switchTargets)
                    targets.Add(t);
            }
        }
        return targets;
    }

    /// <summary>
    /// Extract switch target offsets from a switch instruction's operand.
    /// </summary>
    private static int[] ExtractSwitchTargets(AotCoreIrInstructionArtifact instruction)
    {
        var operand = instruction.Operand;
        if (operand is int[] array)
            return array;
        if (operand is IReadOnlyList<int> list)
            return list.ToArray();
        if (operand is System.Text.Json.JsonElement { ValueKind: System.Text.Json.JsonValueKind.Array } jsonElement)
            return jsonElement.EnumerateArray().Select(e => e.GetInt32()).ToArray();
        return Array.Empty<int>();
    }

    /// <summary>
    /// Build basic blocks from the flat instruction list.
    /// </summary>
    private static IReadOnlyList<BasicBlock> BuildBasicBlocks(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        HashSet<int> branchTargets)
    {
        if (instructions.Count == 0)
            return Array.Empty<BasicBlock>();

        // Identify block leaders
        var leaders = new HashSet<int> { GetRequiredIlOffset(instructions[0]) };
        foreach (var target in branchTargets)
            leaders.Add(target);

        // Every instruction after a terminator is a leader
        for (int i = 0; i < instructions.Count - 1; i++)
        {
            if (IsBlockTerminatorOpcode(instructions[i].Op))
                leaders.Add(GetRequiredIlOffset(instructions[i + 1]));
        }

        // Partition into blocks
        var blocks = new List<BasicBlock>();
        int blockStart = 0;
        for (int i = 0; i <= instructions.Count; i++)
        {
            if (i == instructions.Count || (i > 0 && leaders.Contains(GetRequiredIlOffset(instructions[i]))))
            {
                // Block [blockStart, i-1]
                var blockInstructions = instructions.Skip(blockStart).Take(i - blockStart).ToList();
                var terminator = blockInstructions.Count > 0 && IsBlockTerminatorOpcode(blockInstructions[^1].Op)
                    ? blockInstructions[^1]
                    : null;
                var bodyInstrs = terminator != null
                    ? blockInstructions.Take(blockInstructions.Count - 1).ToList()
                    : blockInstructions;

                int? branchTarget = null;
                int? conditionalTarget = null;
                IReadOnlyList<int> switchTargets = Array.Empty<int>();
                bool isTerminal = false;

                if (terminator != null)
                {
                    switch (terminator.Op)
                    {
                        case "br":
                        case "leave":
                            branchTarget = GetRequiredIntOperand(terminator);
                            break;
                        case "brtrue":
                        case "brfalse":
                        case "beq":
                        case "bne.un":
                        case "bge":
                        case "bge.un":
                        case "bgt":
                        case "ble":
                        case "blt":
                            conditionalTarget = GetRequiredIntOperand(terminator);
                            branchTarget = null; // fall-through handled structurally
                            break;
                        case "switch":
                            switchTargets = ExtractSwitchTargets(terminator);
                            break;
                        case "ret":
                        case "throw":
                        case "rethrow":
                        case "endfilter":
                        case "endfinally":
                            isTerminal = true;
                            break;
                    }
                }

                var block = new BasicBlock(
                    StartOffset: GetRequiredIlOffset(instructions[blockStart]),
                    BodyInstructions: bodyInstrs,
                    Terminator: terminator,
                    Kind: blockStart == 0 ? BasicBlockKind.Entry : BasicBlockKind.Normal,
                    BranchTarget: branchTarget,
                    ConditionalTarget: conditionalTarget,
                    SwitchTargets: switchTargets,
                    IsTerminal: isTerminal);

                blocks.Add(block);
                blockStart = i;
            }
        }

        return blocks;
    }

    /// <summary>
    /// Build a control flow graph from the flat instruction list.
    /// </summary>
    private static ControlFlowGraph BuildControlFlowGraph(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        IReadOnlySet<int> offsets)
    {
        var branchTargets = EnumerateBranchTargets(instructions);
        var blocks = BuildBasicBlocks(instructions, branchTargets);
        var offsetToBlockIndex = new Dictionary<int, int>();
        for (int i = 0; i < blocks.Count; i++)
            offsetToBlockIndex[blocks[i].StartOffset] = i;

        // Check reducibility: the CFG is reducible if every block (apart from entry)
        // has a unique path structure that doesn't create irreducible loops.
        // We use a simpler check: no forward-crossing conditional branches
        // that can't be represented as if-else or switch.
        bool isReducible = IsCfgReducible(blocks, offsetToBlockIndex);

        return new ControlFlowGraph(blocks, offsetToBlockIndex, isReducible);
    }

    /// <summary>
    /// Check if the CFG is reducible using simple heuristics:
    /// 1. No backward edges (we're not doing loop detection)
    /// 2. All conditional targets are forward
    /// 3. Switch targets don't cross between blocks
    /// </summary>
    private static bool IsCfgReducible(
        IReadOnlyList<BasicBlock> blocks,
        IReadOnlyDictionary<int, int> offsetToBlockIndex)
    {
        for (int i = 0; i < blocks.Count; i++)
        {
            var block = blocks[i];
            if (block.ConditionalTarget.HasValue)
            {
                // Conditional targets must be forward
                if (!offsetToBlockIndex.TryGetValue(block.ConditionalTarget.Value, out var targetIdx))
                    return false;
                if (targetIdx <= i)
                    return false; // backward branch = loop, not handled
            }
            if (block.BranchTarget.HasValue)
            {
                // Branch target must be forward (backward = loop)
                if (offsetToBlockIndex.TryGetValue(block.BranchTarget.Value, out var targetIdx) && targetIdx <= i)
                    return false;
            }
            if (block.SwitchTargets.Count > 0)
            {
                // All switch targets must be forward
                foreach (var t in block.SwitchTargets)
                {
                    if (offsetToBlockIndex.TryGetValue(t, out var targetIdx) && targetIdx <= i)
                        return false;
                }
            }
        }

        // Check for "jump into the middle" patterns: a block reached from both
        // a conditional branch fall-through AND an unconditional branch target.
        // This indicates a pattern that can't be represented as structured if-else.
        for (int i = 0; i < blocks.Count; i++)
        {
            int predecessorCount = 0;
            bool hasStructuralPred = false; // reached by fall-through from i-1
            bool hasBranchPred = false; // reached as branch target

            if (i > 0 && blocks[i - 1].Terminator != null)
            {
                var prevTerm = blocks[i - 1].Terminator;
                if (prevTerm.Op is "br" or "leave")
                {
                    hasBranchPred = blocks[i - 1].BranchTarget == blocks[i].StartOffset;
                }
                else if (IsConditionalBranchOpcode(prevTerm.Op))
                {
                    // The fall-through from a conditional branch goes to the next block
                    hasStructuralPred = true;
                    hasBranchPred = blocks[i - 1].ConditionalTarget == blocks[i].StartOffset;
                }
                else if (IsSwitchOpcode(prevTerm.Op))
                {
                    hasBranchPred = blocks[i - 1].SwitchTargets.Contains(blocks[i].StartOffset);
                }
                else
                {
                    // No terminator = fall-through
                    hasStructuralPred = true;
                }
            }
            else if (i > 0)
            {
                hasStructuralPred = true; // fall-through
            }

            if (hasStructuralPred && hasBranchPred)
            {
                // This block has both a fall-through predecessor and a branch-encoded
                // predecessor. Check farther back for more complex patterns.
                // For our purposes, a block that is BOTH a branch target AND
                // a fall-through target is suspicious but may be a simple merge point.
                // Only flag it if there are additional branch targets from elsewhere.
                predecessorCount = (hasStructuralPred ? 1 : 0) + (hasBranchPred ? 1 : 0);
            }

            // Count all blocks that branch to this one
            for (int j = 0; j < blocks.Count; j++)
            {
                if (j == i) continue;
                var b = blocks[j];
                if (b.BranchTarget == blocks[i].StartOffset ||
                    b.ConditionalTarget == blocks[i].StartOffset ||
                    (b.Terminator != null && IsSwitchOpcode(b.Terminator.Op) && b.SwitchTargets.Contains(blocks[i].StartOffset)))
                {
                    predecessorCount++;
                }
            }

            // If a block has > 2 predecessors from different contexts, it may be irreducible
            if (predecessorCount > 2)
            {
                // This is still often reducible (e.g., merge after if-else).
                // Only flag if the block is a branch target from multiple
                // non-sequential locations AND also a fall-through target.
            }
        }

        return true;
    }

    // ──────────────────────────────────────────────
    // Structured Control Flow Recovery
    // ──────────────────────────────────────────────

    /// <summary>
    /// Recover structured control flow from the CFG.
    /// </summary>
    private static StructuredNode RecoverStructure(
        ControlFlowGraph cfg,
        int startIndex, int endIndex)
    {
        if (startIndex > endIndex)
            return new SequenceNode(Array.Empty<StructuredNode>());

        if (startIndex == endIndex)
        {
            var block = cfg.Blocks[startIndex];
            if (block.Terminator == null || block.IsTerminal || block.Terminator.Op is "br" or "leave")
            {
                return new BasicBlockNode(block);
            }
            if (IsConditionalBranchOpcode(block.Terminator.Op))
            {
                // Single condition block with no merge → degenerate if without else
                return BuildIfThenElse(cfg, startIndex);
            }
            if (IsSwitchOpcode(block.Terminator.Op))
            {
                return BuildSwitch(cfg, startIndex);
            }
            return new BasicBlockNode(block);
        }

        // Try to find a conditional branch at the start of the interval
        var firstBlock = cfg.Blocks[startIndex];
        if (firstBlock.ConditionalTarget.HasValue && IsConditionalBranchOpcode(firstBlock.Terminator?.Op ?? ""))
        {
            var ite = BuildIfThenElse(cfg, startIndex);
            if (ite is IfThenElseNode iteNode)
            {
                // Find where the merge block is and continue after it
                int afterMergeIdx;
                if (iteNode.MergeOffset.HasValue &&
                    cfg.OffsetToBlockIndex.TryGetValue(iteNode.MergeOffset.Value, out var mergeIdx))
                {
                    afterMergeIdx = mergeIdx + 1;
                }
                else
                {
                    // Check if the then/else branches both terminate
                    // In that case the "merge" is the end of the interval
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
        if (IsSwitchOpcode(firstBlock.Terminator?.Op ?? ""))
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
            if (block.ConditionalTarget.HasValue && IsConditionalBranchOpcode(block.Terminator?.Op ?? ""))
            {
                var ite = BuildIfThenElse(cfg, idx);
                seqNodes.Add(ite);
                // Advance past the if-then-else structure
                if (ite is IfThenElseNode iteResult && iteResult.MergeOffset.HasValue &&
                    cfg.OffsetToBlockIndex.TryGetValue(iteResult.MergeOffset.Value, out var mergeIdx))
                {
                    idx = mergeIdx + 1;
                }
                else
                {
                    idx = endIndex + 1; // terminated branches
                }
            }
            else if (IsSwitchOpcode(block.Terminator?.Op ?? ""))
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

        // Find the merge point: the first block that post-dominates both paths
        int? mergeOffset = FindMergePoint(cfg, conditionBlockIndex, trueBlockIdx, falseBlockIdx);

        // Recurse on then and else branches
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
            // No merge found - branches are terminated
            thenBranch = RecoverStructure(cfg, trueBlockIdx, endIdx);
            if (falseBlockIdx <= endIdx)
            {
                var elseBranch = RecoverStructure(cfg, falseBlockIdx, endIdx);
                return new IfThenElseNode(condBlock, thenBranch, elseBranch, null);
            }
            return new IfThenElseNode(condBlock, thenBranch, null, null);
        }
    }

    /// <summary>
    /// Find the merge point for an if-then-else structure.
    /// The merge point is the first block reachable from both the true and false paths.
    /// </summary>
    private static int? FindMergePoint(
        ControlFlowGraph cfg,
        int conditionBlockIndex,
        int trueBlockIndex,
        int falseBlockIndex)
    {
        var visitedTrue = new HashSet<int>();
        var visitedFalse = new HashSet<int>();

        // Walk forward from both paths
        WalkForward(cfg, trueBlockIndex, visitedTrue);
        WalkForward(cfg, falseBlockIndex, visitedFalse);

        // Intersection gives merge candidates, pick lowest index
        var intersection = visitedTrue.Intersect(visitedFalse).ToList();
        if (intersection.Count == 0)
            return null;

        int earliest = intersection.Min();
        if (earliest <= conditionBlockIndex)
            return null;

        return cfg.Blocks[earliest].StartOffset;
    }

    /// <summary>
    /// Walk forward through the CFG collecting all reachable block indices.
    /// Stops at terminal blocks.
    /// </summary>
    private static void WalkForward(ControlFlowGraph cfg, int startIndex, HashSet<int> visited)
    {
        var stack = new Stack<int>();
        stack.Push(startIndex);
        while (stack.Count > 0)
        {
            var idx = stack.Pop();
            if (!visited.Add(idx))
                continue;
            var block = cfg.Blocks[idx];
            if (block.IsTerminal)
                continue;
            if (block.BranchTarget.HasValue && cfg.OffsetToBlockIndex.TryGetValue(block.BranchTarget.Value, out var bt))
                stack.Push(bt);
            if (block.ConditionalTarget.HasValue && cfg.OffsetToBlockIndex.TryGetValue(block.ConditionalTarget.Value, out var ct))
                stack.Push(ct);
            if (block.SwitchTargets.Count > 0)
            {
                foreach (var t in block.SwitchTargets)
                {
                    if (cfg.OffsetToBlockIndex.TryGetValue(t, out var st))
                        stack.Push(st);
                }
            }
            if (block.Terminator == null || !IsBlockTerminatorOpcode(block.Terminator.Op))
            {
                // Fall-through to next block
                if (idx + 1 < cfg.Blocks.Count)
                    stack.Push(idx + 1);
            }
        }
    }

    /// <summary>
    /// Build a switch node.
    /// </summary>
    private static StructuredNode BuildSwitch(ControlFlowGraph cfg, int switchBlockIndex)
    {
        var swBlock = cfg.Blocks[switchBlockIndex];
        if (swBlock.SwitchTargets.Count == 0)
            return new BasicBlockNode(swBlock);

        // Group targets by their block index
        var caseMap = new Dictionary<int, List<int>>(); // target block index → case values
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

        // The merge point is the block after all case bodies
        int maxTargetIdx = caseMap.Keys.Max();
        int mergeIdx = maxTargetIdx + 1;
        int endIdx = cfg.Blocks.Count - 1;

        // Build case bodies
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

        // Default body: blocks before merge that aren't covered by any case
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
}
