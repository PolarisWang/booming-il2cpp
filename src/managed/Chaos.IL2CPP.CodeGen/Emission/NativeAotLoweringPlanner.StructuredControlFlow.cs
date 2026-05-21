using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    // ��������������������������������������������������������������������������������������������
    // Data structures for CFG and structured recovery
    // ��������������������������������������������������������������������������������������������

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
        IReadOnlyDictionary<int, NaturalLoopInfo> LoopHeaders,
        bool IsReducible);


    private sealed record NaturalLoopInfo(
        int HeaderIndex,
        HashSet<int> BodyIndices,
        HashSet<int> LatchIndices);

    // ��������������������������������������������������������������������������������������������
    // CFG Construction
    // ��������������������������������������������������������������������������������������������

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

        var dominators = ComputeDominators(blocks);
        var loopHeaders = FindLoopHeaders(blocks, offsetToBlockIndex, dominators);
        bool isReducible = IsCfgReducible(blocks, offsetToBlockIndex, loopHeaders);

        return new ControlFlowGraph(blocks, offsetToBlockIndex, loopHeaders, isReducible);
    }

    /// <summary>
    /// Compute predecessors for each block in the CFG.
    /// </summary>
    private static IReadOnlyList<IReadOnlyList<int>> ComputePredecessors(IReadOnlyList<BasicBlock> blocks)
    {
        var preds = new List<List<int>>();
        for (int i = 0; i < blocks.Count; i++)
            preds.Add(new List<int>());

        for (int i = 0; i < blocks.Count; i++)
        {
            var block = blocks[i];
            if (block.BranchTarget.HasValue)
            {
                if (TryGetBlockIndex(block.BranchTarget.Value, blocks, out var targetIdx))
                    preds[targetIdx].Add(i);
            }
            if (block.ConditionalTarget.HasValue)
            {
                if (TryGetBlockIndex(block.ConditionalTarget.Value, blocks, out var targetIdx))
                    preds[targetIdx].Add(i);
                if (i + 1 < blocks.Count)
                    preds[i + 1].Add(i);
            }
            if (block.SwitchTargets.Count > 0)
            {
                foreach (var t in block.SwitchTargets)
                {
                    if (TryGetBlockIndex(t, blocks, out var targetIdx))
                        preds[targetIdx].Add(i);
                }
            }
            if (block.Terminator == null || (!IsBlockTerminatorOpcode(block.Terminator.Op) && !block.IsTerminal))
            {
                if (i + 1 < blocks.Count)
                    preds[i + 1].Add(i);
            }
        }

        return preds;
    }

    private static bool TryGetBlockIndex(int offset, IReadOnlyList<BasicBlock> blocks, out int index)
    {
        for (int i = 0; i < blocks.Count; i++)
        {
            if (blocks[i].StartOffset == offset)
            {
                index = i;
                return true;
            }
        }
        index = -1;
        return false;
    }

    /// <summary>
    /// Compute dominators using iterative data-flow algorithm.
    /// dom(0) = {0}, dom(i) = {i} U (intersection of dom(p) for all p in preds(i)).
    /// </summary>
    private static bool[][] ComputeDominators(IReadOnlyList<BasicBlock> blocks)
    {
        int n = blocks.Count;
        if (n == 0) return Array.Empty<bool[]>();

        var dominators = new bool[n][];
        for (int i = 0; i < n; i++)
            dominators[i] = new bool[n];

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                dominators[i][j] = true;

        var preds = ComputePredecessors(blocks);

        bool changed = true;
        while (changed)
        {
            changed = false;
            for (int i = 0; i < n; i++)
            {
                if (i == 0)
                {
                    var newDom = new bool[n];
                    newDom[0] = true;
                    if (!BitSetsEqual(dominators[i], newDom))
                    {
                        dominators[i] = newDom;
                        changed = true;
                    }
                    continue;
                }

                bool[] intersection;
                var predList = preds[i];
                if (predList.Count == 0)
                {
                    intersection = new bool[n];
                    intersection[i] = true;
                }
                else
                {
                    intersection = new bool[n];
                    for (int j = 0; j < n; j++)
                        intersection[j] = true;

                    foreach (var p in predList)
                        for (int j = 0; j < n; j++)
                            intersection[j] = intersection[j] && dominators[p][j];

                    intersection[i] = true;
                }

                if (!BitSetsEqual(dominators[i], intersection))
                {
                    dominators[i] = intersection;
                    changed = true;
                }
            }
        }

        return dominators;
    }

    private static bool BitSetsEqual(bool[] a, bool[] b)
    {
        if (a.Length != b.Length) return false;
        for (int i = 0; i < a.Length; i++)
            if (a[i] != b[i]) return false;
        return true;
    }

    /// <summary>
    /// Find all natural loops in the CFG.
    /// </summary>
    private static Dictionary<int, NaturalLoopInfo> FindLoopHeaders(
        IReadOnlyList<BasicBlock> blocks,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        bool[][] dominators)
    {
        var loopHeaders = new Dictionary<int, NaturalLoopInfo>();

        void CheckBackEdge(int sourceIdx, int? targetOffset)
        {
            if (!targetOffset.HasValue) return;
            if (!offsetToBlockIndex.TryGetValue(targetOffset.Value, out var h)) return;
            if (h > sourceIdx) return;
            if (!dominators[sourceIdx][h]) return;

            if (!loopHeaders.ContainsKey(h))
            {
                var body = FindNaturalLoopBody(blocks, offsetToBlockIndex, h, sourceIdx);
                loopHeaders[h] = new NaturalLoopInfo(h, body, new HashSet<int> { sourceIdx });
            }
            else
            {
                loopHeaders[h].LatchIndices.Add(sourceIdx);
                var additionalBody = FindNaturalLoopBody(blocks, offsetToBlockIndex, h, sourceIdx);
                loopHeaders[h].BodyIndices.UnionWith(additionalBody);
            }
        }

        for (int i = 0; i < blocks.Count; i++)
        {
            var block = blocks[i];
            CheckBackEdge(i, block.ConditionalTarget);
            CheckBackEdge(i, block.BranchTarget);
        }

        return loopHeaders;
    }

    /// <summary>
    /// Find the body of a natural loop given header h and latch l.
    /// </summary>
    private static HashSet<int> FindNaturalLoopBody(
        IReadOnlyList<BasicBlock> blocks,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        int headerIndex,
        int latchIndex)
    {
        var body = new HashSet<int> { headerIndex, latchIndex };
        var stack = new Stack<int>();
        stack.Push(latchIndex);

        while (stack.Count > 0)
        {
            var idx = stack.Pop();
            for (int j = 0; j < blocks.Count; j++)
            {
                if (j == idx) continue;
                if (body.Contains(j)) continue;
                if (j == headerIndex) continue;

                var b = blocks[j];
                bool isPred = false;
                if (b.BranchTarget.HasValue && offsetToBlockIndex.TryGetValue(b.BranchTarget.Value, out var bt) && bt == idx)
                    isPred = true;
                if (!isPred && b.ConditionalTarget.HasValue && offsetToBlockIndex.TryGetValue(b.ConditionalTarget.Value, out var ct) && ct == idx)
                    isPred = true;
                if (!isPred && b.SwitchTargets.Count > 0 && b.SwitchTargets.Any(t => offsetToBlockIndex.TryGetValue(t, out var st) && st == idx))
                    isPred = true;
                if (!isPred && (b.Terminator == null || !IsBlockTerminatorOpcode(b.Terminator.Op)))
                {
                    if (j + 1 < blocks.Count && j + 1 == idx)
                        isPred = true;
                }

                if (isPred && body.Add(j))
                    stack.Push(j);
            }
        }

        return body;
    }

    /// <summary>
    /// Check if the CFG is reducible:
    /// 1. All backward branches are natural loops (header dominates latch)
    /// 2. No irreducible loop structures (cross-nesting)
    /// </summary>
    private static bool IsCfgReducible(
        IReadOnlyList<BasicBlock> blocks,
        IReadOnlyDictionary<int, int> offsetToBlockIndex,
        IReadOnlyDictionary<int, NaturalLoopInfo> loopHeaders)
    {
        for (int i = 0; i < blocks.Count; i++)
        {
            var block = blocks[i];
            if (block.ConditionalTarget.HasValue)
            {
                if (!offsetToBlockIndex.TryGetValue(block.ConditionalTarget.Value, out var targetIdx))
                    return false;
                if (targetIdx < i)
                {
                    if (!loopHeaders.ContainsKey(targetIdx))
                        return false;
                    if (!loopHeaders[targetIdx].LatchIndices.Contains(i))
                        return false;
                }
            }
            if (block.BranchTarget.HasValue)
            {
                if (offsetToBlockIndex.TryGetValue(block.BranchTarget.Value, out var targetIdx) && targetIdx < i)
                {
                    if (!loopHeaders.ContainsKey(targetIdx))
                        return false;
                    if (!loopHeaders[targetIdx].LatchIndices.Contains(i))
                        return false;
                }
            }
            if (block.SwitchTargets.Count > 0)
            {
                foreach (var t in block.SwitchTargets)
                {
                    if (offsetToBlockIndex.TryGetValue(t, out var targetIdx) && targetIdx < i)
                    {
                        if (!loopHeaders.ContainsKey(targetIdx))
                            return false;
                    }
                }
            }
        }

        foreach (var kvp in loopHeaders)
        {
            int h1 = kvp.Key;
            var body1 = kvp.Value.BodyIndices;
            foreach (var kvp2 in loopHeaders)
            {
                int h2 = kvp2.Key;
                if (h1 >= h2) continue;
                if (body1.Contains(h2) && kvp2.Value.BodyIndices.Contains(h1))
                    return false;
            }
        }

        return true;
    }
    /// <summary>
    /// Find the merge point for an if-then-else structure.
    /// </summary>
    private static int? FindMergePoint(
        ControlFlowGraph cfg,
        int conditionBlockIndex,
        int trueBlockIndex,
        int falseBlockIndex)
    {
        var visitedTrue = new HashSet<int>();
        var visitedFalse = new HashSet<int>();

        WalkForward(cfg, trueBlockIndex, visitedTrue);
        WalkForward(cfg, falseBlockIndex, visitedFalse);

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
            if (block.Terminator == null || !IsBlockTerminatorOpcode(block.Terminator.Op)
                || IsConditionalBranchOpcode(block.Terminator.Op))
            {
                if (idx + 1 < cfg.Blocks.Count)
                    stack.Push(idx + 1);
            }
        }
    }

    /// <summary>
    /// Walk a StructuredIR tree and remove trailing br/leave that targets a specific offset.
    /// The branch is redundant because structured control flow provides implicit fallthrough.
    /// </summary>
    private static StructuredIRNode RemoveTrailingBranch(StructuredIRNode node, int targetOffset)
    {
        switch (node)
        {
            case IRBlock block when block.Terminator != null &&
                (block.Terminator.Op == "br" || block.Terminator.Op == "leave") &&
                GetRequiredIntOperand(block.Terminator) == targetOffset:
                return new IRBlock(block.BodyInstructions, null);

            case IRSequence seq when seq.Nodes.Count > 0:
            {
                var trimmed = RemoveTrailingBranch(seq.Nodes[^1], targetOffset);
                if (!ReferenceEquals(trimmed, seq.Nodes[^1]))
                {
                    var newNodes = new List<StructuredIRNode>(seq.Nodes);
                    newNodes[^1] = trimmed;
                    return new IRSequence(newNodes);
                }
                return seq;
            }

            case IRIfThenElse ite:
            {
                var thenBody = RemoveTrailingBranch(ite.ThenBody, targetOffset);
                var elseBody = ite.ElseBody is null ? null : RemoveTrailingBranch(ite.ElseBody, targetOffset);
                var postMergeBody = ite.PostMergeBody is null ? null : RemoveTrailingBranch(ite.PostMergeBody, targetOffset);
                if (!ReferenceEquals(thenBody, ite.ThenBody) || !ReferenceEquals(elseBody, ite.ElseBody) || !ReferenceEquals(postMergeBody, ite.PostMergeBody))
                    return new IRIfThenElse(ite.ConditionInstructions, ite.BranchTerminator, thenBody, elseBody, postMergeBody, ite.PreConditionDepth);
                return ite;
            }

            case IRWhileLoop loop:
            {
                var body = RemoveTrailingBranch(loop.Body, targetOffset);
                if (!ReferenceEquals(body, loop.Body))
                    return new IRWhileLoop(loop.ConditionInstructions, loop.ConditionTerminator, body, loop.ExitOffset);
                return loop;
            }

            case IRDoWhileLoop loop:
            {
                var body = RemoveTrailingBranch(loop.Body, targetOffset);
                if (!ReferenceEquals(body, loop.Body))
                    return new IRDoWhileLoop(body, loop.LatchInstructions, loop.LatchTerminator, loop.HeaderOffset, loop.ExitOffset);
                return loop;
            }

            case IRSwitch sw:
            {
                var caseBodies = sw.CaseBodies.ToDictionary(
                    pair => pair.Key,
                    pair => RemoveTrailingBranch(pair.Value, targetOffset));
                var defaultBody = sw.DefaultBody is null ? null : RemoveTrailingBranch(sw.DefaultBody, targetOffset);
                bool changed = sw.DefaultBody is null != (defaultBody is null) ||
                    sw.CaseBodies.Keys.Any(k => !ReferenceEquals(caseBodies[k], sw.CaseBodies[k]));
                if (changed || !ReferenceEquals(defaultBody, sw.DefaultBody))
                    return new IRSwitch(sw.SwitchInstructions, caseBodies, defaultBody, sw.ExitOffset, sw.FallthroughCaseValues);
                return sw;
            }

            case IRExceptionRegion er:
            {
                var tryBody = RemoveTrailingBranch(er.TryBody, targetOffset);
                var handlerBody = RemoveTrailingBranch(er.HandlerBody, targetOffset);
                if (!ReferenceEquals(tryBody, er.TryBody) || !ReferenceEquals(handlerBody, er.HandlerBody))
                    return new IRExceptionRegion(er.Kind, tryBody, handlerBody, er.CatchTypeSubjectId, er.FilterInstructions);
                return er;
            }

            default:
                return node;
        }
    }

    // ��������������������������������������������������������������������������������������������
    // Structured Control Flow Recovery �� StructuredIR
    // ��������������������������������������������������������������������������������������������

    /// <summary>
    /// Recover structured control flow from the CFG as a pure StructuredIR tree.
    /// </summary>
    private static StructuredIRNode RecoverStructure(
        ControlFlowGraph cfg,
        int startIndex, int endIndex,
        int? loopHeaderOffset = null,
        int? loopExitOffset = null)
    {
        if (startIndex > endIndex)
            return new IRSequence(Array.Empty<StructuredIRNode>());

        if (startIndex == endIndex)
        {
            var block = cfg.Blocks[startIndex];

            // Check for break/continue targeting the surrounding loop context
            if (block.Terminator != null && (block.Terminator.Op is "br" or "leave"))
            {
                int target = GetRequiredIntOperand(block.Terminator);
                if (loopExitOffset.HasValue && target == loopExitOffset.Value)
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
                return BuildIfThenElse(cfg, startIndex, endIndex, loopHeaderOffset, loopExitOffset);
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
                var ite = BuildIfThenElse(cfg, startIndex, endIndex, loopHeaderOffset, loopExitOffset);
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
                        nodes.Add(RecoverStructure(cfg, afterMergeIdx, endIndex, loopHeaderOffset, loopExitOffset));
                    }
                    return new IRSequence(nodes);
                }
            }
        }

        // Try to find a switch at the start
        if (IsSwitchOpcode(firstBlock.Terminator?.Op ?? ""))
        {
            var sw = BuildSwitch(cfg, startIndex, loopHeaderOffset, loopExitOffset);
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
                var ite = BuildIfThenElse(cfg, idx, endIndex, loopHeaderOffset, loopExitOffset);
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
                var sw = BuildSwitch(cfg, idx, loopHeaderOffset, loopExitOffset);
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
                // Check for break/continue in sequential block
                var seqBlock = block;
                if (seqBlock.Terminator != null && (seqBlock.Terminator.Op is "br" or "leave"))
                {
                    int seqTarget = GetRequiredIntOperand(seqBlock.Terminator);
                    if (loopExitOffset.HasValue && seqTarget == loopExitOffset.Value)
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
        int? outerLoopExitOffset = null)
    {
        var header = cfg.Blocks[headerIndex];
        var bodyIndicesAfterHeader = loopInfo.BodyIndices.Where(i => i > headerIndex).ToList();
        int bodyStart = bodyIndicesAfterHeader.Count > 0 ? bodyIndicesAfterHeader.Min() : headerIndex + 1;

        int maxLatchIdx = loopInfo.LatchIndices.Max();
        // bodyEnd must cover all body blocks, not just up to maxLatchIdx.
        // A body block after the max latch occurs when a conditional latch
        // falls through to non-latch code that is still within the loop body.
        int bodyEnd = Math.Max(maxLatchIdx, loopInfo.BodyIndices.Max());
        if (bodyEnd > endIndex) bodyEnd = endIndex;

        int exitIdx = bodyEnd + 1;
        while (exitIdx <= endIndex && loopInfo.BodyIndices.Contains(exitIdx))
            exitIdx++;

        int? exitOffset = exitIdx <= endIndex ? cfg.Blocks[exitIdx].StartOffset : null;
        int headerOffset = header.StartOffset;

        bool isWhile;
        BasicBlock? latchBlock = null;

        if (loopInfo.LatchIndices.Contains(headerIndex))
        {
            // Self-loop: header is also a latch (e.g., do-while pattern
            // where the conditional branch targets its own block).
            // Always treat as do-while.
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
    private static StructuredIRNode BuildIfThenElse(
        ControlFlowGraph cfg, int conditionBlockIndex,
        int endIndex,
        int? loopHeaderOffset = null, int? loopExitOffset = null)
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
            thenBranch = RecoverStructure(cfg, trueBlockIdx, mergeIdx - 1, loopHeaderOffset, loopExitOffset);
            // Else range must not overlap with the true-target block.
            int elseEnd = Math.Min(mergeIdx - 1, trueBlockIdx - 1);
            if (falseBlockIdx <= elseEnd)
            {
                elseBranch = RecoverStructure(cfg, falseBlockIdx, elseEnd, loopHeaderOffset, loopExitOffset);
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
            // Else range must not overlap with the true-target block.
            int elseEnd2 = Math.Min(endIdx, trueBlockIdx - 1);
            if (falseBlockIdx <= elseEnd2)
                elseBranch = RecoverStructure(cfg, falseBlockIdx, elseEnd2, loopHeaderOffset, loopExitOffset);
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
            postMergeBody = RecoverStructure(cfg, mergeIdx, endIdx, loopHeaderOffset, loopExitOffset);

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

        // --- Build default body first ---
        StructuredIRNode? defaultBody = null;
        var coveredBlocks = new HashSet<int>(caseMap.Keys);
        var defaultBlocks = Enumerable.Range(switchBlockIndex + 1, mergeIdx - switchBlockIndex - 1)
            .Where(i => !coveredBlocks.Contains(i))
            .ToList();
        if (defaultBlocks.Count > 0)
        {
            defaultBody = RecoverStructure(cfg, defaultBlocks[0], defaultBlocks[^1], loopHeaderOffset, loopExitOffset);

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
                body = RecoverStructure(cfg, caseBlockIdx, bodyEnd, loopHeaderOffset, loopExitOffset);
            else
                body = new IRSequence(Array.Empty<StructuredIRNode>());

            int? switchMergeOffset = mergeIdx <= endIdx ? cfg.Blocks[mergeIdx].StartOffset : null;
            if (switchMergeOffset.HasValue)
                body = RemoveTrailingBranch(body, switchMergeOffset.Value);

            // Detect fallthrough: a case body that has no control flow terminator
            // (empty body or falls through to the next case).
            if (!IsControlFlowTerminator(body) && ci < sortedCaseKeys.Count - 1)
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

    /// <summary>
    /// Find the synthetic offset assigned to a split copy of the given original block index.
    /// </summary>
    private static int? FindCloneOffset(int originalIdx, Dictionary<int, int> splitCopyIndex, IReadOnlyDictionary<int, int> newOffsetMap)
    {
        if (!splitCopyIndex.TryGetValue(originalIdx, out var cloneIdx))
            return null;
        foreach (var kvp in newOffsetMap)
        {
            if (kvp.Value == cloneIdx)
                return kvp.Key;
        }
        return null;
    }
}
