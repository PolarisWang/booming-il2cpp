using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class Program
{
    private static readonly Func<int>[] DefaultProofEntries =
    [
        MixedExecutionProofEntry.Run,
        InterpreterArithmeticProofEntry.Run,
        InterpreterLoweringProofEntry.Run,
        MixedGenericFlowProofEntry.Run,
        MixedExceptionFlowProofEntry.Run,
        MixedDelegateFlowProofEntry.Run,
    ];

    public static int Main(string[] args)
    {
        if (!ChaosSubjectEntryArguments.TryParse(args, out var selection) || selection.IsNone)
        {
            return RunAll(DefaultProofEntries);
        }

        return selection switch
        {
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.MixedExecutionProof } => MixedExecutionProofEntry.Run(),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.InterpreterLoweringProof } => InterpreterLoweringProofEntry.Run(),
            _ => throw new ArgumentOutOfRangeException(nameof(args), "unsupported MixedExecutionFeaturePack entry selection."),
        };
    }

    private static int RunAll(IEnumerable<Func<int>> entries)
    {
        foreach (var entry in entries)
        {
            Assert.Equal(0, entry(), "MixedExecutionFeaturePack proof entry returned non-zero exit code.");
        }

        return 0;
    }
}
