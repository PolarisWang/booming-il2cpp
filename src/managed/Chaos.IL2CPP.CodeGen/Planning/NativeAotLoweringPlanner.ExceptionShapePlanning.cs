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

    /// <summary>
    /// Validates that a partition's instruction sequence would not underflow the eval stack
    /// during structured IR emission. Returns false if any instruction would pop from an
    /// empty stack (signaling that the partition is not self-contained for structured IR).
    /// </summary>
    /// <param name="instructions">The instruction partition to validate.</param>
    /// <param name="initialDepth">Starting stack depth. Use 0 for most partitions; use 1
    /// for catch/filter handler bodies (runtime pushes the exception object).</param>
    private static bool ValidatePartitionStackBalance(
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions,
        int initialDepth)
    {
        int depth = initialDepth;
        foreach (var instr in instructions)
        {
            string op = instr.Op;
            int pops = GetStackPopCount(instr);
            if (depth < pops)
                return false;
            depth -= pops;
            depth += GetStackPushCount(instr);
        }
        return depth >= 0;
    }

    /// <summary>
    /// Returns the number of values an instruction pops from the eval stack.
    /// </summary>
    private static int GetStackPopCount(AotCoreIrInstructionArtifact instr)
    {
        string op = instr.Op;
        switch (op)
        {
            // Pure pops (pop 1)
            case "pop": case "stloc": case "starg": case "initobj":
            case "stsfld": case "throw":
            case "brfalse": case "brtrue":
            case "endfilter":
                return 1;

            // Pop 2 (instance stores, conditional branches)
            case "stfld": case "stobj":
            case "stind.i4": case "stind.i1": case "stind.i2":
            case "stind.i8": case "stind.r4": case "stind.r8": case "stind.ref":
            case "stind.i":
            case "beq": case "bgt": case "blt": case "bge": case "ble":
            case "bne.un": case "bge.un":
                return 2;

            // Pop 3
            case "stelem": case "stelem.i": case "stelem.ref":
            case "stelem.i1": case "stelem.i2": case "stelem.i4": case "stelem.i8":
            case "stelem.r4": case "stelem.r8":
            case "cpblk":
                return 3;

            // Pop 2, push 1 (net -1)
            case "cgt.un": case "ceq": case "cgt": case "clt":
            case "add": case "sub": case "mul": case "div": case "div.un": case "rem": case "rem.un":
            case "shl": case "shr": case "shr.un":
            case "and": case "or": case "xor":
            case "add.ovf": case "sub.ovf": case "mul.ovf": case "add.ovf.un": case "sub.ovf.un": case "mul.ovf.un":
            case "ldelem": case "ldelem.i": case "ldelem.ref": case "ldelema":
                return 2;

            // Pop 1, push 1 (net 0)
            case "ldfld": case "ldflda":
            case "ldind.i4": case "ldind.u1": case "ldind.i1":
            case "ldind.u2": case "ldind.i2": case "ldind.u4":
            case "ldind.i8": case "ldind.r4": case "ldind.r8": case "ldind.ref":
            case "ldind.i":
            case "box": case "unbox": case "unbox.any":
            case "castclass": case "isinst":
            case "ldobj": case "ldlen": case "localloc":
            case "conv.i4": case "conv.i1": case "conv.i2": case "conv.i8":
            case "conv.u8": case "conv.r4": case "conv.r8": case "conv.u":
            case "conv.u1": case "conv.u2": case "conv.u4":
            case "conv.r.un": case "ckfinite":
            case "conv.ovf.i1": case "conv.ovf.u1": case "conv.ovf.i2": case "conv.ovf.u2":
            case "conv.ovf.i4": case "conv.ovf.u4": case "conv.ovf.i8": case "conv.ovf.u8":
            case "conv.ovf.i": case "conv.ovf.u": case "conv.ovf.i8.un": case "conv.ovf.u8.un": case "conv.ovf.i.un": case "conv.ovf.u.un":
            case "conv.ovf.i1.un": case "conv.ovf.i2.un": case "conv.ovf.i4.un":
            case "conv.ovf.u1.un": case "conv.ovf.u2.un": case "conv.ovf.u4.un":
            case "not": case "neg":
            case "mkrefany":
            case "ldvirtftn":
            case "refanyval": case "refanytype":
                return 1;

            // Pop for call and newobj: depends on parameter count
            case "call": case "callvirt": case "calli": case "newobj":
            {
                int? paramCount = instr.TargetParameterCount;
                if (paramCount.HasValue)
                {
                    // newobj: `this` is not on the IL stack
                    return op == "newobj" ? paramCount.Value : paramCount.Value;
                }
                string? callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                if (!string.IsNullOrEmpty(callee))
                {
                    int inferred = InferParameterCountFromSubjectId(callee);
                    return inferred;
                }
                return 0;
            }

            // jmp: special — forwards arguments directly
            case "jmp":
                return 0;

            // ret, br, leave, switch, endfinally: pop count handled below
            case "ret":
                return 0; // ret's pop is return-value dependent, validated separately

            case "switch":
                return 1;

            case "br": case "leave": case "endfinally":
                return 0;

            // Unknown opcode: conservative, assume pops 0
            default:
                return 0;
        }
    }

    /// <summary>
    /// Returns the number of values an instruction pushes onto the eval stack.
    /// </summary>
    private static int GetStackPushCount(AotCoreIrInstructionArtifact instr)
    {
        string op = instr.Op;
        switch (op)
        {
            // Pure pushes (+1)
            case "ldc.i4": case "ldc.i8": case "ldc.r4": case "ldc.r8":
            case "ldarg": case "ldstr": case "ldtoken": case "ldarga":
            case "ldnull": case "ldloc": case "ldloca":
            case "ldsfld": case "ldsflda":
            case "ldftn": case "newarr": case "sizeof":
            case "dup": case "arglist":
                return 1;

            // Pop 2, push 1 (= net -1)
            case "cgt.un": case "ceq": case "cgt": case "clt":
            case "add": case "sub": case "mul": case "div": case "div.un": case "rem": case "rem.un":
            case "shl": case "shr": case "shr.un":
            case "and": case "or": case "xor":
            case "add.ovf": case "sub.ovf": case "mul.ovf": case "add.ovf.un": case "sub.ovf.un": case "mul.ovf.un":
            case "ldelem": case "ldelem.i": case "ldelem.ref": case "ldelema":
                return 1;

            // Pop 1, push 1 (= net 0)
            case "ldfld": case "ldflda":
            case "ldind.i4": case "ldind.u1": case "ldind.i1":
            case "ldind.u2": case "ldind.i2": case "ldind.u4":
            case "ldind.i8": case "ldind.r4": case "ldind.r8": case "ldind.ref":
            case "ldind.i":
            case "box": case "unbox": case "unbox.any":
            case "castclass": case "isinst":
            case "ldobj": case "ldlen": case "localloc":
            case "conv.i4": case "conv.i1": case "conv.i2": case "conv.i8":
            case "conv.u8": case "conv.r4": case "conv.r8": case "conv.u":
            case "conv.u1": case "conv.u2": case "conv.u4":
            case "conv.r.un": case "ckfinite":
            case "conv.ovf.i1": case "conv.ovf.u1": case "conv.ovf.i2": case "conv.ovf.u2":
            case "conv.ovf.i4": case "conv.ovf.u4": case "conv.ovf.i8": case "conv.ovf.u8":
            case "conv.ovf.i": case "conv.ovf.u": case "conv.ovf.i8.un": case "conv.ovf.u8.un": case "conv.ovf.i.un": case "conv.ovf.u.un":
            case "conv.ovf.i1.un": case "conv.ovf.i2.un": case "conv.ovf.i4.un":
            case "conv.ovf.u1.un": case "conv.ovf.u2.un": case "conv.ovf.u4.un":
            case "not": case "neg":
            case "mkrefany":
            case "ldvirtftn":
            case "refanyval": case "refanytype":
                return 1;

            // Pop 2, push 2 (= net 0, box + typeHandle)
            // Actually this is pop 1 push 2 for mkrefany, refanyval is pop 2 push 1
            // Already handled above.

            // Pure pops (push 0)
            case "pop": case "stloc": case "starg": case "initobj":
            case "stsfld": case "throw":
            case "brfalse": case "brtrue":
            case "stfld": case "stobj":
            case "stind.i4": case "stind.i1": case "stind.i2":
            case "stind.i8": case "stind.r4": case "stind.r8": case "stind.ref":
            case "stind.i":
            case "beq": case "bgt": case "blt": case "bge": case "ble":
            case "bne.un": case "bge.un":
            case "stelem": case "stelem.i": case "stelem.ref":
            case "stelem.i1": case "stelem.i2": case "stelem.i4": case "stelem.i8":
            case "stelem.r4": case "stelem.r8":
            case "cpblk":
            case "endfilter":
            case "jmp":
                return 0;

            // ret, br, leave, switch, endfinally
            case "ret":
                return 0;
            case "switch":
                return 0;
            case "br": case "leave": case "endfinally":
                return 0;

            // call/callvirt/calli/newobj: push 0 or 1 depending on return type
            case "call": case "callvirt": case "calli": case "newobj":
            {
                string? callee = instr.Callee ?? instr.TargetReference?.SubjectId;
                string? retType = instr.TargetReturnType;
                if (string.IsNullOrEmpty(retType) && !string.IsNullOrEmpty(callee))
                    retType = InferReturnTypeFromSubjectId(callee);
                if (!string.IsNullOrEmpty(retType) && retType != "System.Void")
                    return 1;
                return op == "newobj" ? 1 : 0;
            }

            default:
                // Unknown opcode: conservative, assume pushes 1
                return 1;
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
