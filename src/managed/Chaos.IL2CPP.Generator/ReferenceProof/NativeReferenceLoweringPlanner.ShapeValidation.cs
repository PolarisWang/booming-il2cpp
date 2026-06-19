using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

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
}
