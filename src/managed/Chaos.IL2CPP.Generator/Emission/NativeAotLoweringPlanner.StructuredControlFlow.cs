using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

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
        or "bgt" or "bgt.un" or "ble" or "ble.un" or "blt" or "blt.un"
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
        or "bgt" or "bgt.un" or "ble" or "ble.un" or "blt" or "blt.un" => true,
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
                        case "bgt.un":
                        case "ble":
                        case "ble.un":
                        case "blt":
                        case "blt.un":
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

            // Standard natural loop: header (target h) dominates latch (source)
            if (dominators[sourceIdx][h])
            {
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
                return;
            }

            // Reversed loop pattern (C# for-loop IL: body before condition):
            // source (condition block) dominates target (body block).
            // The backward edge goes from condition→body (later→earlier in layout),
            // but the actual loop structure has the condition as the header.
            // Register with sourceIdx as the header and h as the latch.
            if (dominators[h][sourceIdx])
            {
                if (!loopHeaders.ContainsKey(sourceIdx))
                {
                    var body = FindNaturalLoopBody(blocks, offsetToBlockIndex, sourceIdx, h);
                    if (!body.Contains(sourceIdx))
                        body.Add(sourceIdx);
                    loopHeaders[sourceIdx] = new NaturalLoopInfo(sourceIdx, body, new HashSet<int> { h });
                }
                else
                {
                    loopHeaders[sourceIdx].LatchIndices.Add(h);
                    var additionalBody = FindNaturalLoopBody(blocks, offsetToBlockIndex, sourceIdx, h);
                    loopHeaders[sourceIdx].BodyIndices.UnionWith(additionalBody);
                }
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
                    // Standard loop: target is header, source is latch
                    if (loopHeaders.ContainsKey(targetIdx) && loopHeaders[targetIdx].LatchIndices.Contains(i))
                        continue;
                    // Reversed loop (body-before-condition pattern): source is header, target is latch
                    if (loopHeaders.ContainsKey(i) && loopHeaders[i].LatchIndices.Contains(targetIdx))
                        continue;
                    // Not a recognized backedge → irreducible
                    return false;
                }
            }
            if (block.BranchTarget.HasValue)
            {
                if (offsetToBlockIndex.TryGetValue(block.BranchTarget.Value, out var targetIdx) && targetIdx < i)
                {
                    // Standard loop: target is header, source is latch
                    if (loopHeaders.ContainsKey(targetIdx) && loopHeaders[targetIdx].LatchIndices.Contains(i))
                        continue;
                    // Reversed loop: source is header, target is latch
                    if (loopHeaders.ContainsKey(i) && loopHeaders[i].LatchIndices.Contains(targetIdx))
                        continue;
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
    /// Walks forward from both branches and finds the first common block
    /// that is reachable from both sides.  The walk is bounded to the
    /// current CFG region (no backtrack below conditionBlockIndex)
    /// so that loop backedges do not produce false merge points.
    /// </summary>
    private static int? FindMergePoint(
        ControlFlowGraph cfg,
        int conditionBlockIndex,
        int trueBlockIndex,
        int falseBlockIndex)
    {
        var visitedTrue = new HashSet<int>();
        var visitedFalse = new HashSet<int>();

        WalkForward(cfg, trueBlockIndex, visitedTrue, conditionBlockIndex);
        WalkForward(cfg, falseBlockIndex, visitedFalse, conditionBlockIndex);

        var intersection = visitedTrue.Intersect(visitedFalse).ToList();
        if (intersection.Count == 0)
            return null;

        int earliest = intersection.Min();
        // The merge point must be strictly after the condition block —
        // reaching the condition block itself means we followed a backedge.
        if (earliest <= conditionBlockIndex)
            return null;

        return cfg.Blocks[earliest].StartOffset;
    }

    /// <summary>
    /// Walk forward through the CFG collecting all reachable block indices.
    /// Bounded by minBlockIndex — edges targeting blocks at or before
    /// minBlockIndex are not followed (this prevents loop backedges
    /// from polluting the reachable set).
    /// </summary>
    private static void WalkForward(ControlFlowGraph cfg, int startIndex, HashSet<int> visited, int minBlockIndex = -1)
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
            if (block.BranchTarget.HasValue && cfg.OffsetToBlockIndex.TryGetValue(block.BranchTarget.Value, out var bt) && bt > minBlockIndex)
                stack.Push(bt);
            if (block.ConditionalTarget.HasValue && cfg.OffsetToBlockIndex.TryGetValue(block.ConditionalTarget.Value, out var ct) && ct > minBlockIndex)
                stack.Push(ct);
            if (block.SwitchTargets.Count > 0)
            {
                foreach (var t in block.SwitchTargets)
                {
                    if (cfg.OffsetToBlockIndex.TryGetValue(t, out var st) && st > minBlockIndex)
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

    // ────────────────────────────────────────────────────────────────────────────
    // Structured Control Flow Recovery → StructuredIR
    // ��������������������������������������������������������������������������������������������

    /// <summary>
    /// Maximum recursion depth for RecoverStructure. Prevents stack overflow
    /// from extremely nested CFGs during structure recovery.
    /// </summary>
    private const int MaxRecoverStructureDepth = 6;

    /// <summary>
    /// Recover structured control flow from the CFG as a pure StructuredIR tree.
    /// </summary>

    /// <summary>
    /// Build a loop node from a natural loop.
    /// </summary>

    /// <summary>
    /// Computes the maximum expected eval-stack depth at the start of the
    /// condition block based on predecessor blocks' net stack contributions.
    /// Values pushed by predecessor CFG blocks are invisible to the structured
    /// slot tracker when it processes the condition instructions, so this
    /// computed depth ensures the slot context has adequate capacity.
    /// </summary>

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
