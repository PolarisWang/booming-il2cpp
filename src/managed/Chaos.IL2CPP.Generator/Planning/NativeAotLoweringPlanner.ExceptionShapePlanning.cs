using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private static bool TryCreateCatchOnlyExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out CatchOnlyExceptionMethodShape? catchOnlyShape)
    {
        catchOnlyShape = null;

        // Must have exactly one catch region with no nesting
        if (method.ExceptionRegions.Count != 1)
            return false;
        var region = method.ExceptionRegions[0];
        if (region.HandlingKindCode != AotCoreIrExceptionRegionKind.Catch)
            return false;

        var (prefix, tryBody, handler, tail) = PartitionInstructionsByOffset(
            method.Instructions,
            region.TryOffset, region.TryLength,
            region.HandlerOffset, region.HandlerLength);

        // Validate partitions: catch handler body starts with exception object on stack,
        // so seed validation with depth=1 for the handler partition.
        if (!ValidatePartitionStackBalance(tryBody, initialDepth: 0) ||
            !ValidatePartitionStackBalance(handler, initialDepth: 1) ||
            !ValidatePartitionStackBalance(tail, initialDepth: 0))
            return false;

        catchOnlyShape = new CatchOnlyExceptionMethodShape(region, prefix, tryBody, handler, tail);
        return true;
    }

    private static bool TryCreateFilterOnlyExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out FilterOnlyExceptionMethodShape? filterOnlyShape)
    {
        filterOnlyShape = null;

        if (method.ExceptionRegions.Count != 1)
            return false;
        var region = method.ExceptionRegions[0];
        if (region.HandlingKindCode != AotCoreIrExceptionRegionKind.Filter)
            return false;
        if (region.FilterOffset == null)
            return false;

        int filterStart = region.FilterOffset.Value;
        int filterEnd = region.HandlerOffset;
        int handlerEnd = region.HandlerOffset + region.HandlerLength;

        var (prefix, tryBody) = PartitionPrefixAndTry(method.Instructions, region.TryOffset, region.TryLength);
        var filterInstructions = GetInstructionsInRange(method.Instructions, filterStart, filterEnd - filterStart);
        var handlerInstructions = GetInstructionsInRange(method.Instructions, region.HandlerOffset, region.HandlerLength);
        var tail = GetInstructionsAfter(method.Instructions, handlerEnd);

        // Filter handler body starts with exception object on stack (depth=1 for handler).
        if (!ValidatePartitionStackBalance(tryBody, initialDepth: 0) ||
            !ValidatePartitionStackBalance(filterInstructions, initialDepth: 1) ||
            !ValidatePartitionStackBalance(handlerInstructions, initialDepth: 1) ||
            !ValidatePartitionStackBalance(tail, initialDepth: 0))
            return false;

        filterOnlyShape = new FilterOnlyExceptionMethodShape(
            region, prefix, tryBody, filterInstructions, handlerInstructions, tail);
        return true;
    }

    private static bool TryCreateFinallyOnlyExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out FinallyOnlyExceptionMethodShape? finallyOnlyShape)
    {
        finallyOnlyShape = null;

        if (method.ExceptionRegions.Count == 0)
            return false;

        // All regions must be Finally, and there must be no nesting of
        // different kinds (catch/filter) inside the finally try blocks.
        foreach (var r in method.ExceptionRegions)
            if (r.HandlingKindCode != AotCoreIrExceptionRegionKind.Finally)
                return false;

        // Sort regions by nesting depth (innermost first = smallest try range first)
        var sorted = method.ExceptionRegions
            .OrderBy(r => r.TryLength)
            .ThenBy(r => r.TryOffset)
            .ToArray();

        // Innermost try range determines the actual try body
        var innermost = sorted[0];
        var (prefix, tryBody) = PartitionPrefixAndTry(method.Instructions, innermost.TryOffset, innermost.TryLength);

        // Map each region's handler to a FinallyHandlerShape
        var handlers = new List<FinallyHandlerShape>(sorted.Length);
        foreach (var r in sorted)
        {
            var handlerInstructions = GetInstructionsInRange(
                method.Instructions, r.HandlerOffset, r.HandlerLength);
            if (!ValidatePartitionStackBalance(handlerInstructions, initialDepth: 0))
                return false;
            handlers.Add(new FinallyHandlerShape(r, handlerInstructions));
        }

        int outermostHandlerEnd = sorted[^1].HandlerOffset + sorted[^1].HandlerLength;
        var tail = GetInstructionsAfter(method.Instructions, outermostHandlerEnd);

        if (!ValidatePartitionStackBalance(tryBody, initialDepth: 0) ||
            !ValidatePartitionStackBalance(tail, initialDepth: 0))
            return false;

        finallyOnlyShape = new FinallyOnlyExceptionMethodShape(prefix, tryBody, handlers, tail);
        return true;
    }

    private static bool TryCreateCatchAndFinallyExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out CatchAndFinallyExceptionMethodShape? catchAndFinallyShape)
    {
        catchAndFinallyShape = null;

        // Pattern: an outer finally wrapping a catch region
        if (method.ExceptionRegions.Count < 2)
            return false;

        // Find the catch region (innermost)
        var catchRegion = method.ExceptionRegions.FirstOrDefault(
            r => r.HandlingKindCode == AotCoreIrExceptionRegionKind.Catch);
        if (catchRegion == null)
            return false;

        // The remaining regions should all be finally
        var finallyRegions = method.ExceptionRegions
            .Where(r => r.HandlingKindCode == AotCoreIrExceptionRegionKind.Finally)
            .OrderBy(r => r.TryLength) // innermost first
            .ToArray();

        if (finallyRegions.Length == 0)
            return false;

        // Ensure catch region's try is within the innermost finally's try
        var innerFinally = finallyRegions[0];
        bool catchInFinally = innerFinally.TryOffset <= catchRegion.TryOffset &&
            innerFinally.TryOffset + innerFinally.TryLength >= catchRegion.TryOffset + catchRegion.TryLength;
        if (!catchInFinally)
            return false;

        // The catch's try body may have a pre-finally-prep zone and a post-try-prep zone.
        // For simplicity, the try body is the instructions protected by the catch.
        var (prefix, innerTryInstructions) = PartitionPrefixAndTry(
            method.Instructions, catchRegion.TryOffset, catchRegion.TryLength);

        // Instructions between catch try start and end (the full range that finally covers)
        int postInnerTryEnd = innerFinally.TryOffset + innerFinally.TryLength;
        var postInnerTry = GetInstructionsInRange(
            method.Instructions, catchRegion.TryOffset + catchRegion.TryLength,
            postInnerTryEnd - (catchRegion.TryOffset + catchRegion.TryLength));

        // Catch handler body
        var handlerInstructions = GetInstructionsInRange(
            method.Instructions, catchRegion.HandlerOffset, catchRegion.HandlerLength);

        // Inner finally (if present — may be optional in some IL patterns)
        FinallyHandlerShape? innerFinallyHandler = null;
        var innerFinallyInstructions = GetInstructionsInRange(
            method.Instructions, innerFinally.HandlerOffset, innerFinally.HandlerLength);
        if (innerFinallyInstructions.Count > 0)
        {
            if (!ValidatePartitionStackBalance(innerFinallyInstructions, initialDepth: 0))
                return false;
            innerFinallyHandler = new FinallyHandlerShape(innerFinally, innerFinallyInstructions);
        }

        // Outer finally handlers (beyond the innermost one)
        var outerFinallyHandlers = new List<FinallyHandlerShape>();
        for (int i = 1; i < finallyRegions.Length; i++)
        {
            var fin = finallyRegions[i];
            var finInstructions = GetInstructionsInRange(
                method.Instructions, fin.HandlerOffset, fin.HandlerLength);
            if (!ValidatePartitionStackBalance(finInstructions, initialDepth: 0))
                return false;
            outerFinallyHandlers.Add(new FinallyHandlerShape(fin, finInstructions));
        }

        int tailStart = finallyRegions[^1].HandlerOffset + finallyRegions[^1].HandlerLength;
        var tail = GetInstructionsAfter(method.Instructions, tailStart);

        // Validate all partitions: catch handler starts with exception (depth=1).
        if (!ValidatePartitionStackBalance(innerTryInstructions, initialDepth: 0) ||
            !ValidatePartitionStackBalance(handlerInstructions, initialDepth: 1) ||
            !ValidatePartitionStackBalance(postInnerTry, initialDepth: 0) ||
            !ValidatePartitionStackBalance(tail, initialDepth: 0))
            return false;

        catchAndFinallyShape = new CatchAndFinallyExceptionMethodShape(
            catchRegion, prefix, Array.Empty<AotCoreIrInstructionArtifact>(),
            innerTryInstructions, innerFinallyHandler,
            postInnerTry, handlerInstructions, outerFinallyHandlers, tail);
        return true;
    }

    private static bool TryCreateFilterAndFinallyExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out FilterAndFinallyExceptionMethodShape? filterAndFinallyShape)
    {
        filterAndFinallyShape = null;

        var filterRegion = method.ExceptionRegions.FirstOrDefault(
            r => r.HandlingKindCode == AotCoreIrExceptionRegionKind.Filter);
        if (filterRegion == null || filterRegion.FilterOffset == null)
            return false;

        var finallyRegions = method.ExceptionRegions
            .Where(r => r.HandlingKindCode == AotCoreIrExceptionRegionKind.Finally)
            .OrderBy(r => r.TryLength)
            .ToArray();

        if (finallyRegions.Length == 0)
            return false;

        int filterStart = filterRegion.FilterOffset.Value;
        int handlerEnd = filterRegion.HandlerOffset + filterRegion.HandlerLength;

        var (prefix, tryBody) = PartitionPrefixAndTry(
            method.Instructions, filterRegion.TryOffset, filterRegion.TryLength);
        var filterInstructions = GetInstructionsInRange(
            method.Instructions, filterStart, filterRegion.HandlerOffset - filterStart);
        var handlerInstructions = GetInstructionsInRange(
            method.Instructions, filterRegion.HandlerOffset, filterRegion.HandlerLength);

        // Finally handlers
        var finallyHandlers = new List<FinallyHandlerShape>();
        foreach (var fin in finallyRegions)
        {
            var finInstructions = GetInstructionsInRange(
                method.Instructions, fin.HandlerOffset, fin.HandlerLength);
            if (!ValidatePartitionStackBalance(finInstructions, initialDepth: 0))
                return false;
            finallyHandlers.Add(new FinallyHandlerShape(fin, finInstructions));
        }

        int tailStart = finallyRegions[^1].HandlerOffset + finallyRegions[^1].HandlerLength;
        if (tailStart < handlerEnd) tailStart = handlerEnd;
        var tail = GetInstructionsAfter(method.Instructions, tailStart);

        // Filter and catch handlers start with exception object on stack (depth=1).
        if (!ValidatePartitionStackBalance(tryBody, initialDepth: 0) ||
            !ValidatePartitionStackBalance(filterInstructions, initialDepth: 1) ||
            !ValidatePartitionStackBalance(handlerInstructions, initialDepth: 1) ||
            !ValidatePartitionStackBalance(tail, initialDepth: 0))
            return false;

        filterAndFinallyShape = new FilterAndFinallyExceptionMethodShape(
            filterRegion, prefix, tryBody, filterInstructions,
            handlerInstructions, finallyHandlers, tail);
        return true;
    }

    private static bool TryCreateMultipleCatchExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out MultipleCatchExceptionMethodShape? multiCatchShape)
    {
        multiCatchShape = null;

        // Collect all catch-type exception regions
        var catchRegions = method.ExceptionRegions
            .Where(r => r.HandlingKindCode == AotCoreIrExceptionRegionKind.Catch)
            .ToList();

        // Must have at least 2 catch regions (1 is handled by CatchOnly)
        if (catchRegions.Count < 2)
            return false;

        // No filter or finally regions in the method
        if (method.ExceptionRegions.Any(r =>
                r.HandlingKindCode != AotCoreIrExceptionRegionKind.Catch))
            return false;

        // Use the first catch region's try range as the shared try body range
        var first = catchRegions[0];
        int tryOffset = first.TryOffset;
        int tryLength = first.TryLength;
        int tryEnd = tryOffset + tryLength;

        // Validate all catch regions share compatible try ranges
        foreach (var r in catchRegions)
        {
            if (r.TryOffset != tryOffset || r.TryLength != tryLength)
                return false;
        }

        // Partition: prefix (before try), try body, handler bodies, tail
        var prefix = new List<AotCoreIrInstructionArtifact>();
        var tryBody = new List<AotCoreIrInstructionArtifact>();
        var handlerInstructionsList = new List<IReadOnlyList<AotCoreIrInstructionArtifact>>();
        var tail = new List<AotCoreIrInstructionArtifact>();

        int maxHandlerEnd = 0;
        foreach (var r in catchRegions)
        {
            int handlerEnd = r.HandlerOffset + r.HandlerLength;
            if (handlerEnd > maxHandlerEnd)
                maxHandlerEnd = handlerEnd;
        }

        foreach (var instr in method.Instructions)
        {
            int off = instr.IlOffset;
            if (off < tryOffset)
                prefix.Add(instr);
            else if (off < tryEnd)
                tryBody.Add(instr);
            else if (off >= maxHandlerEnd)
                tail.Add(instr);
        }

        StripTrailingLeaveInstructions(tryBody);

        // Extract each catch handler's instructions
        foreach (var r in catchRegions)
        {
            var handlerInstructions = GetInstructionsInRange(
                method.Instructions, r.HandlerOffset, r.HandlerLength);
            var handlerList = new List<AotCoreIrInstructionArtifact>(handlerInstructions);
            StripTrailingLeaveInstructions(handlerList);
            handlerInstructionsList.Add(handlerList);
        }

        // Validate stack balance: try body starts at depth 0, each catch handler
        // starts at depth 1 (runtime pushes the exception object), tail at depth 0.
        if (!ValidatePartitionStackBalance(tryBody, initialDepth: 0))
            return false;

        foreach (var handler in handlerInstructionsList)
        {
            if (!ValidatePartitionStackBalance(handler, initialDepth: 1))
                return false;
        }

        if (!ValidatePartitionStackBalance(tail, initialDepth: 0))
            return false;

        multiCatchShape = new MultipleCatchExceptionMethodShape(
            catchRegions, prefix, tryBody, handlerInstructionsList, tail);
        return true;
    }

    private static bool TryCreateGenericExceptionMethodShape(
        AotCoreIrMethodArtifact method,
        out GenericExceptionMethodShape? genericShape)
    {
        genericShape = null;

        if (method.ExceptionRegions.Count == 0)
            return false;

        // Sort by nesting depth: innermost first (smallest try range first).
        // Regions are expected to be cleanly nested (one try range fully contained
        // within another) or disjoint.  The IL spec prohibits partial overlap.
        var sorted = method.ExceptionRegions
            .OrderBy(r => r.TryLength)
            .ThenBy(r => r.TryOffset)
            .ToList();

        // Determine outermost try bounds for prefix/tail extraction
        int outerTryStart = sorted.Min(r => r.TryOffset);
        int outerTryEnd = sorted.Max(r => r.TryOffset + r.TryLength);
        int outerHandlerEnd = sorted.Max(r => r.HandlerOffset + r.HandlerLength);
        int outermostEnd = Math.Max(outerTryEnd, outerHandlerEnd);

        // Prefix: instructions before the outermost try range
        var prefix = method.Instructions
            .Where(i => i.IlOffset < outerTryStart)
            .ToList();
        if (!ValidatePartitionStackBalance(prefix, 0))
            return false;

        // Tail: instructions after all handler ranges
        var tail = method.Instructions
            .Where(i => i.IlOffset >= outermostEnd)
            .ToList();
        if (!ValidatePartitionStackBalance(tail, 0))
            return false;

        // Build region entries from innermost to outermost.
        // Track IL offset ranges consumed by inner regions' try + handler
        // so each outer region's try body excludes already-covered instructions.
        var entries = new List<GenericExceptionRegionEntry>();
        var consumedRanges = new List<(int start, int end)>();
        int maxHandlerPushes = 0;

        foreach (var region in sorted)
        {
            int tryStart = region.TryOffset;
            int tryEnd = tryStart + region.TryLength;
            int handlerStart = region.HandlerOffset;
            int handlerEnd = handlerStart + region.HandlerLength;

            // Try instructions: in try range, not in any consumed range
            var tryInstructions = method.Instructions
                .Where(i => i.IlOffset >= tryStart && i.IlOffset < tryEnd &&
                            !consumedRanges.Any(c => i.IlOffset >= c.start && i.IlOffset < c.end))
                .ToList();
            StripTrailingLeaveInstructions(tryInstructions);

            // Handler instructions
            var handlerInstructions = new List<AotCoreIrInstructionArtifact>(
                method.Instructions.Where(i => i.IlOffset >= handlerStart && i.IlOffset < handlerEnd));
            StripTrailingLeaveInstructions(handlerInstructions);

            // Filter instructions (only for filter regions)
            IReadOnlyList<AotCoreIrInstructionArtifact>? filterInstructions = null;
            if (region.HandlingKindCode == AotCoreIrExceptionRegionKind.Filter && region.FilterOffset != null)
            {
                filterInstructions = method.Instructions
                    .Where(i => i.IlOffset >= region.FilterOffset.Value && i.IlOffset < region.HandlerOffset)
                    .ToList();
            }

            // Validate stack balance: catch/filter handlers start with exception object (depth=1)
            bool handlerHasExceptionPush = region.HandlingKindCode != AotCoreIrExceptionRegionKind.Finally;
            if (!ValidatePartitionStackBalance(tryInstructions, 0) ||
                !ValidatePartitionStackBalance(handlerInstructions, handlerHasExceptionPush ? 1 : 0))
                return false;

            // Mark this region's ranges as consumed so outer regions skip them
            consumedRanges.Add((tryStart, tryEnd));
            consumedRanges.Add((handlerStart, handlerEnd));

            entries.Add(new GenericExceptionRegionEntry(
                region, tryInstructions, handlerInstructions, filterInstructions));

            int pushes = region.HandlingKindCode switch
            {
                AotCoreIrExceptionRegionKind.Catch => 1,
                AotCoreIrExceptionRegionKind.Filter => 2,
                _ => 0
            };
            if (pushes > maxHandlerPushes) maxHandlerPushes = pushes;
        }

        genericShape = new GenericExceptionMethodShape(prefix, tail, entries, maxHandlerPushes);
        return true;
    }

    // ── Instruction partitioning helpers ──

    /// <summary>
    /// Partitions instructions into prefix/try/handler/tail based on IL offset ranges.
    /// Terminal leave instructions are excluded from try/handler partitions since
    /// structured IR handles control flow via block terminators.
    /// </summary>
    private static (IReadOnlyList<AotCoreIrInstructionArtifact> Prefix,
                    IReadOnlyList<AotCoreIrInstructionArtifact> TryBody,
                    IReadOnlyList<AotCoreIrInstructionArtifact> Handler,
                    IReadOnlyList<AotCoreIrInstructionArtifact> Tail)
        PartitionInstructionsByOffset(
            IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
            int tryOffset, int tryLength,
            int handlerOffset, int handlerLength)
    {
        int tryEnd = tryOffset + tryLength;
        int handlerEnd = handlerOffset + handlerLength;

        var prefix = new List<AotCoreIrInstructionArtifact>();
        var tryBody = new List<AotCoreIrInstructionArtifact>();
        var handler = new List<AotCoreIrInstructionArtifact>();
        var tail = new List<AotCoreIrInstructionArtifact>();

        foreach (var instr in instructions)
        {
            int off = instr.IlOffset;
            if (off < tryOffset)
                prefix.Add(instr);
            else if (off < tryEnd)
                tryBody.Add(instr);
            else if (off >= handlerOffset && off < handlerEnd)
                handler.Add(instr);
            else if (off >= handlerEnd)
                tail.Add(instr);
        }

        // Remove trailing leave instructions from tryBody — structured IR
        // handles control flow via block terminators, not explicit leave.
        StripTrailingLeaveInstructions(tryBody);
        StripTrailingLeaveInstructions(handler);

        return (prefix, tryBody, handler, tail);
    }

    private static (IReadOnlyList<AotCoreIrInstructionArtifact> Prefix,
                    IReadOnlyList<AotCoreIrInstructionArtifact> TryBody)
        PartitionPrefixAndTry(
            IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
            int tryOffset, int tryLength)
    {
        int tryEnd = tryOffset + tryLength;
        var prefix = new List<AotCoreIrInstructionArtifact>();
        var tryBody = new List<AotCoreIrInstructionArtifact>();

        foreach (var instr in instructions)
        {
            if (instr.IlOffset < tryOffset)
                prefix.Add(instr);
            else if (instr.IlOffset < tryEnd)
                tryBody.Add(instr);
        }

        StripTrailingLeaveInstructions(tryBody);
        return (prefix, tryBody);
    }

    /// <summary>
    /// Validates that a partition's instruction sequence would not underflow the eval stack
    /// during structured IR emission. Returns false if any instruction would pop from an
    /// empty stack (signaling that the partition is not self-contained for structured IR).
    /// </summary>
    /// <param name="instructions">The instruction partition to validate.</param>
    /// <param name="initialDepth">Starting stack depth. Use 0 for most partitions; use 1
    /// for catch/filter handler bodies (runtime pushes the exception object).</param>

    /// <summary>
    /// Returns the number of values an instruction pops from the eval stack.
    /// </summary>

    /// <summary>
    /// Returns the number of values an instruction pushes onto the eval stack.
    /// </summary>

    // ── TryCreateFinallyHandlerEmissionPlan (kept from original) ──
}
