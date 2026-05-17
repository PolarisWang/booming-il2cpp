using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

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
            handlers.Add(new FinallyHandlerShape(r, handlerInstructions));
        }

        int outermostHandlerEnd = sorted[^1].HandlerOffset + sorted[^1].HandlerLength;
        var tail = GetInstructionsAfter(method.Instructions, outermostHandlerEnd);

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
            innerFinallyHandler = new FinallyHandlerShape(innerFinally, innerFinallyInstructions);

        // Outer finally handlers (beyond the innermost one)
        var outerFinallyHandlers = new List<FinallyHandlerShape>();
        for (int i = 1; i < finallyRegions.Length; i++)
        {
            var fin = finallyRegions[i];
            var finInstructions = GetInstructionsInRange(
                method.Instructions, fin.HandlerOffset, fin.HandlerLength);
            outerFinallyHandlers.Add(new FinallyHandlerShape(fin, finInstructions));
        }

        int tailStart = finallyRegions[^1].HandlerOffset + finallyRegions[^1].HandlerLength;
        var tail = GetInstructionsAfter(method.Instructions, tailStart);

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
            finallyHandlers.Add(new FinallyHandlerShape(fin, finInstructions));
        }

        int tailStart = finallyRegions[^1].HandlerOffset + finallyRegions[^1].HandlerLength;
        if (tailStart < handlerEnd) tailStart = handlerEnd;
        var tail = GetInstructionsAfter(method.Instructions, tailStart);

        filterAndFinallyShape = new FilterAndFinallyExceptionMethodShape(
            filterRegion, prefix, tryBody, filterInstructions,
            handlerInstructions, finallyHandlers, tail);
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

    private static IReadOnlyList<AotCoreIrInstructionArtifact> GetInstructionsInRange(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int offset, int length)
    {
        int end = offset + length;
        var result = new List<AotCoreIrInstructionArtifact>();
        foreach (var instr in instructions)
        {
            if (instr.IlOffset >= offset && instr.IlOffset < end)
                result.Add(instr);
        }
        return result;
    }

    private static IReadOnlyList<AotCoreIrInstructionArtifact> GetInstructionsAfter(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int offset)
    {
        var result = new List<AotCoreIrInstructionArtifact>();
        foreach (var instr in instructions)
        {
            if (instr.IlOffset >= offset)
                result.Add(instr);
        }
        return result;
    }

    private static void StripTrailingLeaveInstructions(List<AotCoreIrInstructionArtifact> instructions)
    {
        while (instructions.Count > 0 &&
               string.Equals(instructions[^1].Op, "leave", StringComparison.Ordinal))
        {
            instructions.RemoveAt(instructions.Count - 1);
        }
    }

    // ── TryCreateFinallyHandlerEmissionPlan (kept from original) ──

    private static bool TryCreateFinallyHandlerEmissionPlan(
        FinallyHandlerShape handlerShape,
        out FinallyHandlerEmissionPlan? emissionPlan)
    {
        emissionPlan = null;
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions = handlerShape.Instructions;
        if (instructions.Count == 0) return false;

        if (!string.Equals(instructions[instructions.Count - 1].Op, "endfinally", StringComparison.Ordinal))
            return false;

        var body = instructions.Take(instructions.Count - 1).ToArray();
        if (body.Length == 0)
        {
            emissionPlan = new FinallyHandlerEmissionPlan(null, Array.Empty<AotCoreIrInstructionArtifact>());
            return true;
        }

        // Look for a single brtrue/brfalse guard at the end of the body
        var guards = body
            .Select((instruction, index) => (instruction, index))
            .Where(entry => string.Equals(entry.instruction.Op, "brtrue", StringComparison.Ordinal) ||
                            string.Equals(entry.instruction.Op, "brfalse", StringComparison.Ordinal))
            .ToArray();

        if (guards.Length == 0)
        {
            emissionPlan = new FinallyHandlerEmissionPlan(null, body);
            return true;
        }

        if (guards.Length != 1)
            return false;

        var (guardInst, guardIdx) = guards[0];
        if (GetRequiredIntOperand(guardInst) != GetRequiredIlOffset(instructions[instructions.Count - 1]))
            return false;

        var condInstructions = body.Take(guardIdx).ToArray();
        var bodyInstructions = body.Skip(guardIdx + 1).ToArray();
        if (condInstructions.Length == 0)
            return false;

        emissionPlan = new FinallyHandlerEmissionPlan(
            new FinallyHandlerGuardShape(condInstructions, string.Equals(guardInst.Op, "brtrue", StringComparison.Ordinal)),
            bodyInstructions);
        return true;
    }
}
