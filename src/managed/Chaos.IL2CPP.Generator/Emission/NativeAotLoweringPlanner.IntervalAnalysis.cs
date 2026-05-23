using System;
using System.Collections.Generic;
using System.Linq;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Interval in a control flow graph (derived via T1-T2 analysis).
    /// Each interval has a single header node; the body contains all nodes
    /// reachable from the header without passing through another interval header.
    /// </summary>
    private sealed record Interval(int HeaderIndex, HashSet<int> BodyIndices)
    {
        /// <summary>
        /// Index of the outer interval in the interval graph, if any.
        /// Set during interval graph construction.
        /// </summary>
        public int? OuterIntervalIndex { get; set; }

        /// <summary>
        /// Indices of inner intervals (sub-intervals) nested within this one.
        /// Populated during interval graph construction.
        /// </summary>
        public List<int> InnerIntervalIndices { get; } = new();
    }

    /// <summary>
    /// Interval graph derived from a control flow graph via T1-T2 analysis.
    /// Each node in this graph corresponds to an Interval in the original CFG.
    /// </summary>
    private sealed record IntervalGraph(
        List<Interval> Intervals,
        /// <summary>Maps original block index → interval index that contains it.</summary>
        Dictionary<int, int> BlockToInterval,
        /// <summary>Edges between intervals: intervalIndex → set of successor interval indices.</summary>
        Dictionary<int, HashSet<int>> Edges,
        bool IsReducible)
    {
        public bool HasIntervals => Intervals.Count > 0;
    }

    /// <summary>
    /// Attempt to make an irreducible CFG reducible via interval analysis and
    /// interval-based node splitting.  More powerful than ApplyNodeSplitting
    /// because interval analysis handles cross-nesting irreducible patterns
    /// that simple header cloning cannot resolve.
    /// </summary>
    private static ControlFlowGraph MakeCfgReducibleViaIntervalAnalysis(ControlFlowGraph cfg)
    {
        // Phase 1: Build interval graph from the current CFG
        var intervalGraph = BuildIntervalGraph(cfg);

        // If the interval graph is already reducible, no splitting needed
        if (intervalGraph.IsReducible)
            return cfg;

        // Phase 2: Find irreducible intervals and apply node splitting
        return SplitIrreducibleIntervals(cfg, intervalGraph);
    }

    /// <summary>
    /// Build an interval graph from a control flow graph using T1-T2 analysis.
    ///
    /// Algorithm (from Hecht & Ullman, "Characterizations of Reducible Flow Graphs"):
    ///   1. Start with the entry node as the header of the first interval.
    ///   2. For each interval, iteratively apply T1 and T2 until closure:
    ///      - T1: Remove self-loop edges (they are implicit in a structured loop)
    ///      - T2: Add a node N to the interval if all predecessors of N are
    ///            already in the interval (or N is the header)
    ///   3. After closing an interval, start a new interval from the next
    ///      unclaimed node that is reachable from previous interval headers.
    ///   4. The resulting intervals form a DAG (interval graph) if the CFG is reducible.
    ///      If cycles exist in the interval graph, the CFG is irreducible.
    /// </summary>
    private static IntervalGraph BuildIntervalGraph(ControlFlowGraph cfg)
    {
        var blocks = cfg.Blocks;
        int n = blocks.Count;
        if (n == 0)
            return new IntervalGraph(new List<Interval>(), new Dictionary<int, int>(), new Dictionary<int, HashSet<int>>(), true);

        var preds = ComputePredecessors(blocks);

        // Maps block index → interval index that contains it
        var blockToInterval = new Dictionary<int, int>();
        var intervals = new List<Interval>();

        // Set of block indices not yet assigned to any interval
        var unassigned = new HashSet<int>(Enumerable.Range(0, n));

        // Phase 1: Build intervals via T1-T2 closure
        var worklist = new Queue<int>();
        worklist.Enqueue(0); // Start from entry block

        while (worklist.Count > 0 && unassigned.Count > 0)
        {
            int header = worklist.Dequeue();
            if (!unassigned.Contains(header))
                continue;

            // Start a new interval with this header
            var body = new HashSet<int> { header };
            var intervalNodes = new HashSet<int> { header };
            unassigned.Remove(header);

            // T1-T2 closure: iteratively add nodes until no more can be added
            bool changed = true;
            while (changed)
            {
                changed = false;

                // Check each unassigned node for T1 (self-loop) — handled implicitly
                // since we track self-loops at the CFG level.

                // T2: add node if all predecessors are in the interval
                foreach (int candidate in unassigned.ToList())
                {
                    if (AllPredecessorsInInterval(candidate, preds, intervalNodes))
                    {
                        intervalNodes.Add(candidate);
                        body.Add(candidate);
                        unassigned.Remove(candidate);
                        changed = true;
                    }
                }
            }

            // Record the interval
            int intervalIndex = intervals.Count;
            intervals.Add(new Interval(header, body));

            foreach (int b in body)
                blockToInterval[b] = intervalIndex;

            // Find new headers for subsequent intervals:
            // Any unassigned node that has a predecessor (in any interval) is reachable
            var reachableUnassigned = unassigned
                .Where(u => preds[u].Any(p => !unassigned.Contains(p)))
                .ToList();

            // Also include any node that is a branch target from nodes in this interval
            // but is not yet assigned
            foreach (int bodyNode in body)
            {
                var b = blocks[bodyNode];
                CheckTargetForNewHeader(b.BranchTarget, cfg, unassigned, worklist);
                CheckTargetForNewHeader(b.ConditionalTarget, cfg, unassigned, worklist);
                foreach (var st in b.SwitchTargets)
                    CheckTargetForNewHeader(st, cfg, unassigned, worklist);
            }

            // Add reachable unassigned nodes as potential headers
            foreach (var r in reachableUnassigned)
            {
                if (!worklist.Contains(r))
                    worklist.Enqueue(r);
            }
        }

        // If any nodes remain unassigned, each becomes its own interval
        foreach (int residual in unassigned)
        {
            int intervalIndex = intervals.Count;
            intervals.Add(new Interval(residual, new HashSet<int> { residual }));
            blockToInterval[residual] = intervalIndex;
        }

        // Phase 2: Build interval graph edges
        var edges = new Dictionary<int, HashSet<int>>();
        for (int i = 0; i < intervals.Count; i++)
            edges[i] = new HashSet<int>();

        for (int i = 0; i < n; i++)
        {
            int srcInterval = blockToInterval[i];
            var block = blocks[i];

            void AddIntervalEdge(int? targetOffset)
            {
                if (!targetOffset.HasValue) return;
                if (!cfg.OffsetToBlockIndex.TryGetValue(targetOffset.Value, out var targetIdx)) return;
                int dstInterval = blockToInterval[targetIdx];
                if (srcInterval != dstInterval)
                    edges[srcInterval].Add(dstInterval);
            }

            AddIntervalEdge(block.BranchTarget);
            AddIntervalEdge(block.ConditionalTarget);
            foreach (var st in block.SwitchTargets)
                AddIntervalEdge(st);
        }

        // Also consider fall-through edges
        for (int i = 0; i < n - 1; i++)
        {
            int srcInterval = blockToInterval[i];
            var block = blocks[i];
            if (!IsBlockTerminatorOpcode(block.Terminator?.Op ?? "") || block.IsTerminal)
                continue;
            // Fall-through to i+1
            int dstInterval = blockToInterval[i + 1];
            if (srcInterval != dstInterval)
                edges[srcInterval].Add(dstInterval);
        }

        // Phase 3: Set up outer/inner interval relationships
        for (int i = 0; i < intervals.Count; i++)
        {
            // An interval A is an outer interval of B if B's header is in A's body
            // (but B is not A itself)
            for (int j = 0; j < intervals.Count; j++)
            {
                if (i == j) continue;
                if (intervals[i].BodyIndices.Contains(intervals[j].HeaderIndex))
                {
                    intervals[j].OuterIntervalIndex = i;
                    intervals[i].InnerIntervalIndices.Add(j);
                }
            }
        }

        // Phase 4: Check reducibility of interval graph
        // A CFG is reducible iff the interval graph has no cycles
        // (i.e., the interval graph is a DAG)
        bool isReducible = IsIntervalGraphAcyclic(intervals, edges);

        return new IntervalGraph(intervals, blockToInterval, edges, isReducible);
    }

    private static bool AllPredecessorsInInterval(
        int nodeIndex,
        IReadOnlyList<IReadOnlyList<int>> predecessors,
        HashSet<int> intervalNodes)
    {
        var preds = predecessors[nodeIndex];
        if (preds.Count == 0)
            return false;
        // T2 condition: ALL predecessors must be in the interval
        // (except for the header, which is handled separately)
        return preds.All(p => intervalNodes.Contains(p));
    }

    private static void CheckTargetForNewHeader(
        int? targetOffset,
        ControlFlowGraph cfg,
        HashSet<int> unassigned,
        Queue<int> worklist)
    {
        if (!targetOffset.HasValue) return;
        if (!cfg.OffsetToBlockIndex.TryGetValue(targetOffset.Value, out var targetIdx)) return;
        if (unassigned.Contains(targetIdx) && !worklist.Contains(targetIdx))
            worklist.Enqueue(targetIdx);
    }

    /// <summary>
    /// Check if the interval graph is acyclic (and thus the CFG is reducible).
    /// Uses DFS-based cycle detection.
    /// </summary>
    private static bool IsIntervalGraphAcyclic(
        List<Interval> intervals,
        Dictionary<int, HashSet<int>> edges)
    {
        int n = intervals.Count;
        if (n == 0) return true;

        var visited = new bool[n];
        var inStack = new bool[n];

        bool Dfs(int node)
        {
            visited[node] = true;
            inStack[node] = true;

            if (edges.TryGetValue(node, out var successors))
            {
                foreach (var succ in successors)
                {
                    if (succ < 0 || succ >= n) continue;
                    if (inStack[succ]) return false; // Cycle detected
                    if (!visited[succ] && !Dfs(succ)) return false;
                }
            }

            inStack[node] = false;
            return true;
        }

        for (int i = 0; i < n; i++)
        {
            if (!visited[i] && !Dfs(i))
                return false;
        }

        return true;
    }

    /// <summary>
    /// Apply node splitting at the interval level to make an irreducible CFG reducible.
    /// For each irreducible interval (one that participates in a cycle in the interval
    /// graph), split its header node by cloning all blocks that have external
    /// (non-interval) predecessors, redirecting those edges to the clones.
    ///
    /// This is more powerful than ApplyNodeSplitting because:
    /// 1. It considers the whole interval graph, not just individual blocks
    /// 2. It can resolve cross-nesting patterns (cycle in interval graph)
    /// 3. It handles switch-based irreducible patterns through interval analysis
    /// </summary>
    private static ControlFlowGraph SplitIrreducibleIntervals(
        ControlFlowGraph cfg,
        IntervalGraph intervalGraph)
    {
        if (intervalGraph.IsReducible)
            return cfg;

        var blocks = new List<BasicBlock>(cfg.Blocks);
        var offsetToBlockIndex = new Dictionary<int, int>(cfg.OffsetToBlockIndex);
        var loopHeaders = new Dictionary<int, NaturalLoopInfo>(cfg.LoopHeaders);

        // Identify intervals that participate in cycles in the interval graph
        var cycleParticipants = FindCycleParticipantIntervals(intervalGraph);
        if (cycleParticipants.Count == 0)
            return cfg; // No irreducible intervals found (shouldn't happen)

        // Collect all blocks that need splitting from the cycle-participant intervals.
        // A block needs splitting if it has incoming edges from blocks OUTSIDE
        // its interval (i.e., external predecessors).
        var allBlocks = cfg.Blocks;
        var preds = ComputePredecessors(allBlocks);
        var splitCandidates = new HashSet<int>();
        var intervalBlockSets = new Dictionary<int, HashSet<int>>();

        foreach (var intIdx in cycleParticipants)
        {
            if (intIdx < 0 || intIdx >= intervalGraph.Intervals.Count)
                continue;
            var interval = intervalGraph.Intervals[intIdx];
            var bodySet = interval.BodyIndices;
            intervalBlockSets[intIdx] = bodySet;

            foreach (int b in bodySet)
            {
                bool hasExternalPred = preds[b].Any(p => !bodySet.Contains(p));
                if (hasExternalPred && b != interval.HeaderIndex)
                {
                    // Non-header block with external predecessors:
                    // split by cloning and redirecting external edges
                    splitCandidates.Add(b);
                }
            }

            // Always split the header if it has multiple distinct entry points
            // from outside the interval (the definition of interval irreducibility)
            int header = interval.HeaderIndex;
            int externalEntryCount = preds[header].Count(p => !bodySet.Contains(p));
            if (externalEntryCount > 1)
            {
                splitCandidates.Add(header);
            }
        }

        if (splitCandidates.Count == 0)
        {
            // Fallback: if no split candidates found in cycle participants,
            // try splitting all headers of cycle-participant intervals
            foreach (int intIdx in cycleParticipants)
            {
                if (intIdx >= 0 && intIdx < intervalGraph.Intervals.Count)
                    splitCandidates.Add(intervalGraph.Intervals[intIdx].HeaderIndex);
            }
        }

        // Apply node splitting to all candidates
        return ApplyMultiNodeSplitting(
            cfg, splitCandidates, preds, blocks, offsetToBlockIndex, loopHeaders);
    }

    /// <summary>
    /// Find intervals that participate in cycles in the interval graph.
    /// These are the intervals on paths that form cycles.
    /// </summary>
    private static HashSet<int> FindCycleParticipantIntervals(IntervalGraph graph)
    {
        int n = graph.Intervals.Count;
        var participants = new HashSet<int>();

        if (n == 0) return participants;

        var visited = new bool[n];
        var inStack = new bool[n];
        var edges = graph.Edges;

        void Dfs(int node)
        {
            visited[node] = true;
            inStack[node] = true;

            if (edges.TryGetValue(node, out var successors))
            {
                foreach (var succ in successors)
                {
                    if (succ < 0 || succ >= n) continue;
                    if (inStack[succ])
                    {
                        // Cycle found: add all nodes on the stack from succ to node
                        participants.Add(succ);
                        participants.Add(node);
                        // Also add the edge source
                        participants.Add(node);
                    }
                    if (!visited[succ])
                        Dfs(succ);
                }
            }

            inStack[node] = false;
        }

        for (int i = 0; i < n; i++)
        {
            if (!visited[i])
                Dfs(i);
        }

        return participants;
    }

    /// <summary>
    /// Apply node splitting to multiple candidates in a single pass.
    /// This extends the original ApplyNodeSplitting to handle multiple
    /// split targets with their per-target predecessor sets.
    /// </summary>
    private static ControlFlowGraph ApplyMultiNodeSplitting(
        ControlFlowGraph originalCfg,
        HashSet<int> splitTargets,
        IReadOnlyList<IReadOnlyList<int>> preds,
        List<BasicBlock> baseBlocks,
        Dictionary<int, int> baseOffsetMap,
        Dictionary<int, NaturalLoopInfo> baseLoopHeaders)
    {
        if (splitTargets.Count == 0)
            return originalCfg;

        int nextSynthOffset = -1;
        var newBlocks = new List<BasicBlock>();
        var newOffsetMap = new Dictionary<int, int>();
        var splitCopyIndex = new Dictionary<int, int>();

        bool IsForwardEdge(int sourceIdx, int? targetOffset)
        {
            if (!targetOffset.HasValue) return false;
            if (!baseOffsetMap.TryGetValue(targetOffset.Value, out var targetIdx)) return false;
            return sourceIdx < targetIdx;
        }

        // Pass 1: build blocks with clones for split targets
        for (int i = 0; i < baseBlocks.Count; i++)
        {
            if (!splitTargets.Contains(i))
            {
                newOffsetMap[baseBlocks[i].StartOffset] = newBlocks.Count;
                newBlocks.Add(baseBlocks[i]);
                continue;
            }

            var original = baseBlocks[i];
            int synthOffset = nextSynthOffset--;

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

            newOffsetMap[original.StartOffset] = newBlocks.Count;
            newBlocks.Add(original);
        }

        // Pass 2: redirect forward edges from non-latch predecessors to clones
        for (int i = 0; i < baseBlocks.Count; i++)
        {
            if (!newOffsetMap.TryGetValue(baseBlocks[i].StartOffset, out var newIdx))
                continue;

            var b = newBlocks[newIdx];

            int? newBranchTarget = b.BranchTarget;
            if (IsForwardEdge(i, b.BranchTarget) &&
                baseOffsetMap.TryGetValue(b.BranchTarget!.Value, out var btIdx) &&
                splitTargets.Contains(btIdx))
            {
                newBranchTarget = FindCloneOffset(btIdx, splitCopyIndex, newOffsetMap);
            }

            int? newConditionalTarget = b.ConditionalTarget;
            if (IsForwardEdge(i, b.ConditionalTarget) &&
                baseOffsetMap.TryGetValue(b.ConditionalTarget!.Value, out var ctIdx) &&
                splitTargets.Contains(ctIdx))
            {
                newConditionalTarget = FindCloneOffset(ctIdx, splitCopyIndex, newOffsetMap);
            }

            IReadOnlyList<int> newSwitchTargets = b.SwitchTargets;
            if (b.SwitchTargets.Count > 0)
            {
                var switched = false;
                var list = new List<int>(b.SwitchTargets.Count);
                foreach (var st in b.SwitchTargets)
                {
                    if (baseOffsetMap.TryGetValue(st, out var stIdx) &&
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

        // Pass 3: Recompute CFG properties
        var newDominators = ComputeDominators(newBlocks);
        var newLoopHeaders = FindLoopHeaders(newBlocks, newOffsetMap, newDominators);
        bool reducible = IsCfgReducible(newBlocks, newOffsetMap, newLoopHeaders);

        // If still not reducible after one round, try one more round with interval analysis
        if (!reducible)
        {
            var result = new ControlFlowGraph(newBlocks, newOffsetMap, newLoopHeaders, reducible);
            var newIntervalGraph = BuildIntervalGraph(result);
            if (!newIntervalGraph.IsReducible)
            {
                // Second round: split additional candidates from the new interval graph
                var newCycleParts = FindCycleParticipantIntervals(newIntervalGraph);
                if (newCycleParts.Count > 0)
                {
                    var newSplitTargets = new HashSet<int>();
                    foreach (var intIdx in newCycleParts)
                    {
                        if (intIdx >= 0 && intIdx < newIntervalGraph.Intervals.Count)
                            newSplitTargets.Add(newIntervalGraph.Intervals[intIdx].HeaderIndex);
                    }
                    if (newSplitTargets.Count > 0)
                    {
                        var round2Preds = ComputePredecessors(newBlocks);
                        return ApplyMultiNodeSplitting(
                            result, newSplitTargets, round2Preds,
                            newBlocks, newOffsetMap, newLoopHeaders);
                    }
                }
            }
            return result;
        }

        return new ControlFlowGraph(newBlocks, newOffsetMap, newLoopHeaders, reducible);
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
