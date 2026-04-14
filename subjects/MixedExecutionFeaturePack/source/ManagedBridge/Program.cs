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

    private static readonly IReadOnlyDictionary<string, Func<int>> DeclaredProofEntriesBySourceEntry =
        new Dictionary<string, Func<int>>(StringComparer.Ordinal)
        {
            ["MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()"] = MixedExecutionProofEntry.Run,
            ["MixedExecutionFeaturePack/InterpreterArithmeticProofEntry::Run()"] = InterpreterArithmeticProofEntry.Run,
            ["MixedExecutionFeaturePack/InterpreterLoweringProofEntry::Run()"] = InterpreterLoweringProofEntry.Run,
            ["MixedExecutionFeaturePack/MixedGenericFlowProofEntry::Run()"] = MixedGenericFlowProofEntry.Run,
            ["MixedExecutionFeaturePack/MixedExceptionFlowProofEntry::Run()"] = MixedExceptionFlowProofEntry.Run,
            ["MixedExecutionFeaturePack/MixedDelegateFlowProofEntry::Run()"] = MixedDelegateFlowProofEntry.Run,
        };

    public static int Main(string[] args)
    {
        if (ChaosSourceEntryArguments.TryParse(args, out var sourceEntrySelection) && !sourceEntrySelection.IsNone)
        {
            return RunSelectedProof(sourceEntrySelection.SourceEntry);
        }

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

    private static int RunSelectedProof(string sourceEntry)
    {
        if (DeclaredProofEntriesBySourceEntry.TryGetValue(sourceEntry, out var entry))
        {
            return entry();
        }

        throw new ArgumentOutOfRangeException(nameof(sourceEntry), "unsupported MixedExecutionFeaturePack source entry.");
    }
}
