using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
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

}
