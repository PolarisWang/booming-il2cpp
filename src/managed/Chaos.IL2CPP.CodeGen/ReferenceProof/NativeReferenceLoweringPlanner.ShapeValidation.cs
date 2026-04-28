using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceLoweringPlanner
{
    private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(TypedIlMethodArtifact method)
    {
        if (method.Blocks.Count != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects a single block in '{method.SubjectId}', but found {method.Blocks.Count}");
        }

        return method.Blocks[0].Instructions;
    }

    private static IReadOnlyList<ManagedInstructionModel>? TryGetSingleBlockInstructions(ManagedMethodModel? method)
    {
        if (method is null || method.Body.Blocks.Count != 1)
        {
            return null;
        }

        return method.Body.Blocks[0].Instructions;
    }

    private static bool IsConstructorThenInstanceCallEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 6 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsConstructorThenInstanceCallProofEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return IsConstructorThenInstanceCallEntryPointShape(instructions);
    }

    private static bool IsConstructorThenInstanceCallEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 6 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsConstructorThenInstanceCallProofEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsConstructorThenInstanceCallEntryPointShape(instructions);
    }

    private static bool IsStaticCallCtorGetterEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 7 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) &&
               IsCallLikeOp(instructions[3].Op) &&
               string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayBoxingReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 16 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "box", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayLengthOnlyEntryPointShape(instructions) ||
               IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayIndexedEntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 20 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayIndexedEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 22 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[21].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 25 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[21].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[22].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[23].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[24].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(instructions) ||
               IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 20 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayReverseReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayReverseRangeReferenceArrayEntryPointShape(instructions) ||
               IsArrayReverseWholeReferenceArrayEntryPointShape(instructions);
    }

    private static bool IsArrayReverseRangeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 17 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayReverseWholeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 15 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayClearReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayClearRangeReferenceArrayEntryPointShape(instructions) ||
               IsArrayClearWholeReferenceArrayEntryPointShape(instructions);
    }

    private static bool IsArrayClearRangeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 19 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayClearWholeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 17 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayBoxingReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 16 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "box", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return IsArrayCopyReferenceArrayLengthOnlyEntryPointShape(instructions) ||
               IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayIndexedEntryPointShape(instructions);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(instructions) ||
               IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 20 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayReverseReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return IsArrayReverseRangeReferenceArrayEntryPointShape(instructions) ||
               IsArrayReverseWholeReferenceArrayEntryPointShape(instructions);
    }

    private static bool IsArrayReverseRangeReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 17 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayReverseWholeReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 15 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayClearReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return IsArrayClearRangeReferenceArrayEntryPointShape(instructions) ||
               IsArrayClearWholeReferenceArrayEntryPointShape(instructions);
    }

    private static bool IsArrayClearRangeReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 19 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayClearWholeReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 17 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 20 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayIndexedEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 22 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[21].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 25 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[21].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[22].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[23].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[24].Op, "ret", StringComparison.Ordinal);
    }

    private static string? TryResolveDevirtualizedCallTarget(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int instructionIndex,
        string declaredTargetSubjectId)
    {
        if (instructionIndex <= 0)
        {
            return null;
        }

        var precedingInstruction = instructions[instructionIndex - 1];
        if (!string.Equals(precedingInstruction.Op, "newobj", StringComparison.Ordinal) ||
            string.IsNullOrWhiteSpace(precedingInstruction.Callee))
        {
            return null;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(precedingInstruction.Callee);
        var declaredTypeSubjectId = GetDeclaringTypeSubjectId(declaredTargetSubjectId);
        if (string.Equals(constructorTypeSubjectId, declaredTypeSubjectId, StringComparison.Ordinal))
        {
            return declaredTargetSubjectId;
        }

        var declaredMethod = GetRequiredMethod(methods, declaredTargetSubjectId);
        var exactMatch = methods.FirstOrDefault(candidate =>
            string.Equals(GetDeclaringTypeSubjectId(candidate.SubjectId), constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(GetMethodName(candidate.SubjectId), GetMethodName(declaredMethod.SubjectId), StringComparison.Ordinal) &&
            string.Equals(candidate.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) &&
            candidate.Parameters.Select(parameter => parameter.Type).SequenceEqual(
                declaredMethod.Parameters.Select(parameter => parameter.Type),
                StringComparer.Ordinal))
            ?.SubjectId;
        if (!string.IsNullOrWhiteSpace(exactMatch))
        {
            return exactMatch;
        }

        return methods.FirstOrDefault(candidate =>
            string.Equals(GetDeclaringTypeSubjectId(candidate.SubjectId), constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(GetMethodName(candidate.SubjectId), GetMethodName(declaredMethod.SubjectId), StringComparison.Ordinal) &&
            string.Equals(candidate.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
            ?.SubjectId;
    }

    private static bool IsPInvokeDllImportMinimalEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 5 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal);
    }

    private static string ValidateConstructorThenInstanceCallEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (IsConstructorThenInstanceCallEntryPointShape(instructions))
        {
            RequireCapability(method, "requires-console-string-output");
            RequireInstructionCount(method, instructions, 6);
            RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
            RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
            RequireInstructionOp(instructions[2], "callvirt", method.SubjectId, 2);
            RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
            RequireInstructionOp(instructions[4], "ldc.i4", method.SubjectId, 4);
            RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);
            RequireInstructionCallee(instructions[3], ConsoleWriteLineStringIcall, method.SubjectId, 3);

            if (GetRequiredOperandInt(instructions[4]) != 0)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
            }

            return ConsoleWriteLineStringIcall;
        }

        throw new InvalidOperationException(
            $"native-reference emitter expects '{method.SubjectId}' to follow constructor-then-instance-call proof shape");
    }

    private static void ValidateStaticCallCtorGetterEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireCapability(method, "requires-console-string-output");
        RequireInstructionCount(method, instructions, 7);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "newobj", method.SubjectId, 2);
        RequireInstructionOpLike(instructions[3], method.SubjectId, 3, "call", "callvirt");
        RequireInstructionOp(instructions[4], "call", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ldc.i4", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);
        RequireInstructionCallee(instructions[4], ConsoleWriteLineStringIcall, method.SubjectId, 4);

        if (GetRequiredOperandInt(instructions[5]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static void ValidateConstructorShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "constructor", "has-canonical-body");
        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldarg", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "ldarg", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "stfld", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);

        if (GetRequiredOperandInt(instructions[0]) != 0 ||
            GetRequiredOperandInt(instructions[2]) != 0 ||
            GetRequiredOperandInt(instructions[3]) != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to use ldarg 0/0/1 pattern");
        }

        _ = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
    }

    private static void ValidateFieldBackedStringInstanceMethodShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (instructions.Count == 7)
        {
            if (string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal))
            {
                RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
                if (GetRequiredOperandInt(instructions[1]) != 0)
                {
                    throw new InvalidOperationException(
                        $"native-reference emitter expects '{method.SubjectId}' literal-prefix shape to load ldarg 0 after the prefix literal");
                }
            }
            else
            {
                RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
                RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
                if (GetRequiredOperandInt(instructions[0]) != 1 ||
                    GetRequiredOperandInt(instructions[1]) != 0)
                {
                    throw new InvalidOperationException(
                        $"native-reference emitter expects '{method.SubjectId}' parameter-prefix shape to use ldarg 1 / ldarg 0 before the captured field load");
                }
            }

            RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
            RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
            RequireInstructionOp(instructions[4], "ldstr", method.SubjectId, 4);
            RequireInstructionOp(instructions[5], "call", method.SubjectId, 5);
            RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);

            RequireInstructionCallee(
                instructions[3],
                StringConcatPairIcall,
                StringConcatPairMethodSubjectId,
                method.SubjectId,
                3);
            RequireInstructionCallee(
                instructions[5],
                StringConcatPairIcall,
                StringConcatPairMethodSubjectId,
                method.SubjectId,
                5);
            return;
        }

        if (instructions.Count == 6)
        {
            RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
            RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
            RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
            RequireInstructionOp(instructions[3], "ldstr", method.SubjectId, 3);
            RequireInstructionOp(instructions[4], "call", method.SubjectId, 4);
            RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);

            if (GetRequiredOperandInt(instructions[1]) != 0)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
            }

            RequireInstructionCallee(
                instructions[4],
                StringConcatTripleIcall,
                StringConcatTripleMethodSubjectId,
                method.SubjectId,
                4);
            return;
        }

        throw new InvalidOperationException(
            $"native-reference emitter expects '{method.SubjectId}' field-backed string instance method to use 6 or 7 canonical instructions");
    }

    private static void ValidateSingleArgumentForwarderShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-forwarder", "has-canonical-body");
        RequireInstructionCount(method, instructions, 2);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ret", method.SubjectId, 1);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to forward ldarg 0");
        }
    }

    private static void ValidateDelegateClosedTargetRelayEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");

        if (!IsDelegateClosedTargetRelayEntryPointShape(instructions))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to follow delegate-closed-target-relay proof shape");
        }

        var writeLineInstructionIndex = GetDelegateClosedTargetRelayWriteLineInstructionIndex(instructions);
        if (!IsConsoleWriteLineStringTarget(
                GetRequiredInstructionCallee(
                    instructions[writeLineInstructionIndex],
                    method.SubjectId,
                    writeLineInstructionIndex)))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to call Console.WriteLine(System.String)");
        }

        var returnValueInstructionIndex = GetDelegateClosedTargetRelayReturnValueInstructionIndex(instructions);
        if (GetRequiredOperandInt(instructions[returnValueInstructionIndex]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static bool IsDelegateClosedTargetRelayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ldftn", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldsfld", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "brtrue", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "pop", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldnull", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldftn", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "stsfld", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static int GetDelegateClosedTargetRelayStaticMethodInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 10
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayMessagePrefixInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 15
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayWriteLineInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 18
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayReturnValueInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 19
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static void ValidateDelegateClosedTargetRelayStaticTailShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 4);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "call", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "ret", method.SubjectId, 3);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to load ldarg 0 before appending a trailing literal");
        }

        var concatIcall = NormalizeStringConcatIcall(GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2));
        if (!string.Equals(concatIcall, StringConcatPairIcall, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects delegate static tail '{method.SubjectId}' to use pair string concat");
        }
    }

    private static string GetDelegateClosedTargetRelayTrailingLiteral(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        ValidateDelegateClosedTargetRelayStaticTailShape(method, instructions);
        return GetRequiredOperandString(instructions[1]);
    }

    private static string GetCapturedStateInstanceMessageSuffixLiteral(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count switch
        {
            7 => GetRequiredOperandString(instructions[4]),
            6 => GetRequiredOperandString(instructions[3]),
            _ => throw new InvalidOperationException(
                $"native-reference emitter cannot extract suffix literal from '{method.SubjectId}'"),
        };
    }

    private static string GetCapturedStateInstanceMessageConcatIcall(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count switch
        {
            7 => GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3),
            6 => GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4),
            _ => throw new InvalidOperationException(
                $"native-reference emitter cannot extract concat icall from '{method.SubjectId}'"),
        };
    }

    private static void ValidateFieldGetterShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-field-getter", "has-canonical-body");
        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldfld", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ret", method.SubjectId, 2);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
        }
    }

    private static void RequireMethodContract(
        TypedIlMethodArtifact method,
        string expectedMethodRole,
        string expectedBodyAvailability)
    {
        if (!string.Equals(method.MethodRole, expectedMethodRole, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have method role '{expectedMethodRole}', but found '{method.MethodRole}'");
        }

        if (!string.Equals(method.BodyAvailability, expectedBodyAvailability, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have body availability '{expectedBodyAvailability}', but found '{method.BodyAvailability}'");
        }
    }

    private static void RequireCapability(
        TypedIlMethodArtifact method,
        string requiredCapability)
    {
        if (!method.Capabilities.Contains(requiredCapability, StringComparer.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to carry capability '{requiredCapability}'");
        }
    }

    private static void RequireInstructionCount(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int expectedCount)
    {
        if (instructions.Count != expectedCount)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have {expectedCount} instructions, but found {instructions.Count}");
        }
    }

    private static void RequireInstructionOp(
        TypedIlInstructionArtifact instruction,
        string expectedOp,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be '{expectedOp}', but found '{instruction.Op}'");
        }
    }

    private static void RequireInstructionOpLike(
        TypedIlInstructionArtifact instruction,
        string subjectId,
        int instructionIndex,
        params string[] expectedOps)
    {
        if (expectedOps.Any(expectedOp => string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal)))
        {
            return;
        }

        throw new InvalidOperationException(
            $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be one of [{string.Join(", ", expectedOps)}], but found '{instruction.Op}'");
    }

    private static bool IsCallLikeOp(string? op)
    {
        return string.Equals(op, "call", StringComparison.Ordinal) ||
               string.Equals(op, "callvirt", StringComparison.Ordinal);
    }

    private static bool IsSupportedArrayCopyMethodSubjectId(string subjectId)
    {
        return (subjectId.Contains("/System.Array::Copy:", StringComparison.Ordinal) &&
                (subjectId.EndsWith("(System.Array,System.Array,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Array,System.Int64)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int64,System.Array,System.Int64,System.Int64)", StringComparison.Ordinal))) ||
               (subjectId.Contains("/System.Array::ConstrainedCopy:", StringComparison.Ordinal) &&
                subjectId.EndsWith("(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", StringComparison.Ordinal)) ||
               (subjectId.Contains("/System.Array::CopyTo:", StringComparison.Ordinal) &&
                (subjectId.EndsWith("(System.Array,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int64)", StringComparison.Ordinal)));
    }

    private static bool IsSupportedArrayReverseMethodSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.Array::Reverse", StringComparison.Ordinal);
    }

    private static bool IsSupportedArrayClearMethodSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.Array::Clear:", StringComparison.Ordinal) &&
               (subjectId.EndsWith("(System.Array,System.Int32,System.Int32)", StringComparison.Ordinal) ||
                subjectId.EndsWith("(System.Array)", StringComparison.Ordinal));
    }

    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Callee, expectedCallee, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call '{expectedCallee}', but found '{instruction.Callee ?? "<null>"}'");
        }
    }

    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string alternateExpectedCallee,
        string subjectId,
        int instructionIndex)
    {
        if (string.Equals(instruction.Callee, expectedCallee, StringComparison.Ordinal) ||
            string.Equals(instruction.Callee, alternateExpectedCallee, StringComparison.Ordinal))
        {
            return;
        }

        throw new InvalidOperationException(
            $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call '{expectedCallee}' or '{alternateExpectedCallee}', but found '{instruction.Callee ?? "<null>"}'");
    }

    private static string GetRequiredInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string subjectId,
        int instructionIndex)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to carry a callee");
        }

        return instruction.Callee;
    }

    private static string GetRequiredOperandString(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            string value => value,
            JsonElement element when element.ValueKind == JsonValueKind.String => element.GetString()
                ?? throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
            _ => throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
        };
    }

    private static int GetRequiredOperandInt(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            int value => value,
            long value => checked((int)value),
            JsonElement element when element.ValueKind == JsonValueKind.Number => element.GetInt32(),
            _ => throw new InvalidOperationException($"expected integer operand for '{instruction.Op}'"),
        };
    }

    private static int GetRequiredPromotedInt64ConstantAsInt(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int constantInstructionIndex,
        string subjectId)
    {
        RequireInstructionOp(instructions[constantInstructionIndex], "ldc.i4", subjectId, constantInstructionIndex);
        RequireInstructionOp(instructions[constantInstructionIndex + 1], "conv.i8", subjectId, constantInstructionIndex + 1);
        return GetRequiredOperandInt(instructions[constantInstructionIndex]);
    }

    private static string NormalizeStringConcatIcall(string? callee)
    {
        if (string.Equals(callee, StringConcatPairIcall, StringComparison.Ordinal) ||
            string.Equals(callee, StringConcatPairMethodSubjectId, StringComparison.Ordinal))
        {
            return StringConcatPairIcall;
        }

        if (string.Equals(callee, StringConcatTripleIcall, StringComparison.Ordinal) ||
            string.Equals(callee, StringConcatTripleMethodSubjectId, StringComparison.Ordinal))
        {
            return StringConcatTripleIcall;
        }

        return callee ?? StringConcatPairIcall;
    }

    private static bool IsConsoleWriteLineStringTarget(string? callee)
    {
        return string.Equals(callee, ConsoleWriteLineStringIcall, StringComparison.Ordinal) ||
               string.Equals(callee, ConsoleWriteLineStringMethodSubjectId, StringComparison.Ordinal);
    }

    private static string GetDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract declaring type from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
    }

    private static bool IsInterfaceDispatchTarget(
        IReadOnlyList<ManagedTypeModel> types,
        string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        return types.Any(type =>
            string.Equals(type.SubjectId, declaringTypeSubjectId, StringComparison.Ordinal) &&
            type.IsInterface);
    }

    private static bool LooksLikeInterfaceMethodSubjectId(string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var typeName = declaringTypeSubjectId[(declaringTypeSubjectId.LastIndexOfAny(['/', '.']) + 1)..];
        return typeName.StartsWith("I", StringComparison.Ordinal) &&
               typeName.Length > 1 &&
               char.IsUpper(typeName[1]);
    }

    private static string GetMethodName(string subjectId)
    {
        var methodSeparatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':');
        var parameterSeparatorIndex = subjectId.IndexOf('(', methodSeparatorIndex + 2);
        if (methodSeparatorIndex <= 0 || parameterSeparatorIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        var methodNameEndIndex = returnTypeSeparatorIndex > methodSeparatorIndex + 1 &&
                                 returnTypeSeparatorIndex < parameterSeparatorIndex
            ? returnTypeSeparatorIndex
            : parameterSeparatorIndex;
        if (methodNameEndIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        return subjectId[(methodSeparatorIndex + 2)..methodNameEndIndex];
    }

    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        string subjectId,
        string description)
    {
        return GetRequiredMetadataRegistration(
            registrations,
            registrationKind,
            registration => string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal),
            description);
    }

    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        Func<MetadataRegistrationEntry, bool> predicate,
        string description)
    {
        var registration = registrations.FirstOrDefault(item =>
            string.Equals(item.RegistrationKind, registrationKind, StringComparison.Ordinal) &&
            predicate(item));
        if (registration is null)
        {
            throw new InvalidOperationException(
                $"missing required '{registrationKind}' metadata registration for {description}");
        }

        return registration;
    }

    private static string GetRequiredRegistrationName(MetadataRegistrationEntry registration)
    {
        if (!string.IsNullOrWhiteSpace(registration.Name))
        {
            return registration.Name;
        }

        throw new InvalidOperationException(
            $"metadata registration '{registration.SubjectId}' is missing name metadata");
    }

    private static string GetRequiredRegistrationDisplayName(MetadataRegistrationEntry registration)
    {
        if (!string.IsNullOrWhiteSpace(registration.DisplayName))
        {
            return registration.DisplayName;
        }

        if (!string.IsNullOrWhiteSpace(registration.Name))
        {
            return registration.Name;
        }

        throw new InvalidOperationException(
            $"metadata registration '{registration.SubjectId}' is missing displayName metadata");
    }

    private static bool IsCompilerGeneratedMemberName(string? name)
    {
        return string.IsNullOrWhiteSpace(name) || name.StartsWith("<", StringComparison.Ordinal);
    }

    private static bool IsSpecialMethodName(string? name)
    {
        if (string.IsNullOrWhiteSpace(name))
        {
            return true;
        }

        return name.StartsWith(".", StringComparison.Ordinal) ||
               name.StartsWith("<", StringComparison.Ordinal) ||
               name.StartsWith("get_", StringComparison.Ordinal) ||
               name.StartsWith("set_", StringComparison.Ordinal) ||
               name.StartsWith("add_", StringComparison.Ordinal) ||
               name.StartsWith("remove_", StringComparison.Ordinal);
    }

    private static uint GetRequiredMetadataToken(
        MetadataRegistrationArtifact metadataRegistration,
        string registrationKind,
        string subjectId)
    {
        uint rowIndex = 0u;

        foreach (var registration in metadataRegistration.Registrations)
        {
            if (!string.Equals(registration.RegistrationKind, registrationKind, StringComparison.Ordinal))
            {
                continue;
            }

            rowIndex++;
            if (string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal))
            {
                return GetMetadataTokenPrefix(registrationKind) | rowIndex;
            }
        }

        throw new InvalidOperationException(
            $"missing required '{registrationKind}' metadata registration for '{subjectId}'");
    }

    private static string CreateTypeTokenLiteral(
        MetadataRegistrationArtifact metadataRegistration,
        string subjectId)
    {
        var hasConcreteRegistration = metadataRegistration.Registrations.Any(item =>
            string.Equals(item.RegistrationKind, "type", StringComparison.Ordinal) &&
            string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        return hasConcreteRegistration
            ? FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", subjectId))
            : FormatCppTokenLiteral(CreatePseudoTypeToken(subjectId));
    }

    private static uint CreatePseudoTypeToken(string subjectId)
    {
        const uint typeTokenPrefix = 0x02000000u;
        uint hash = 2166136261u;

        foreach (var current in subjectId)
        {
            hash ^= current;
            hash *= 16777619u;
        }

        var rowIndex = hash & 0x00FFFFFFu;
        if (rowIndex == 0u)
        {
            rowIndex = 1u;
        }

        return typeTokenPrefix | rowIndex;
    }

    private static uint GetMetadataTokenPrefix(string registrationKind)
    {
        return registrationKind switch
        {
            "type" => 0x02000000u,
            "field" => 0x04000000u,
            "method" => 0x06000000u,
            _ => throw new InvalidOperationException(
                $"native-reference emitter does not know how to derive metadata token for registration kind '{registrationKind}'"),
        };
    }

    private static string FormatCppTokenLiteral(uint token)
    {
        return $"0x{token:X8}u";
    }

    private static string ToCppStringLiteral(string value)
    {
        var builder = new StringBuilder(value.Length + 2);
        builder.Append('"');

        foreach (var current in value)
        {
            builder.Append(current switch
            {
                '\\' => "\\\\",
                '"' => "\\\"",
                '\r' => "\\r",
                '\n' => "\\n",
                '\t' => "\\t",
                _ => current.ToString(),
            });
        }

        builder.Append('"');
        return builder.ToString();
    }

    private static TypedIlMethodArtifact GetRequiredMethod(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        string subjectId)
    {
        var method = methods.FirstOrDefault(item => string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        if (method is null)
        {
            throw new InvalidOperationException($"missing required typed-il method for '{subjectId}'");
        }

        return method;
    }

    private static CodeRegistrationEntry GetRequiredRegistration(
        IReadOnlyList<CodeRegistrationEntry> registrations,
        string subjectId)
    {
        var registration = registrations.FirstOrDefault(item => string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        if (registration is null)
        {
            throw new InvalidOperationException($"missing required method pointer registration for '{subjectId}'");
        }

        return registration;
    }

    private static T LoadRequiredJson<T>(string path)
    {
        if (!File.Exists(path))
        {
            throw new FileNotFoundException("required Stage 4 input artifact is missing", path);
        }

        var value = JsonSerializer.Deserialize<T>(File.ReadAllText(path), JsonOptions);
        if (value is null)
        {
            throw new InvalidOperationException($"failed to deserialize required JSON artifact: {path}");
        }

        return value;
    }
}
