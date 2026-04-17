using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class Program
{
    private static readonly Func<int>[] DefaultProofEntries =
    [
        HotUpdateSkeletonProofEntry.Run,
        PatchIntegrityProofEntry.Run,
        PatchCallbackFlowProofEntry.Run,
        MetadataSupplementProofEntry.Run,
        MethodReplacementProofEntry.Run,
        SharedContractProofEntry.Run,
        VersionRollbackProofEntry.Run,
    ];

    public static int Main(string[] args)
    {
        if (!ChaosSubjectEntryArguments.TryParse(args, out var selection) || selection.IsNone)
        {
            return RunAll(DefaultProofEntries);
        }

        return selection switch
        {
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.HotUpdateSkeletonProof } => HotUpdateSkeletonProofEntry.Run(),
            _ => throw new ArgumentOutOfRangeException(nameof(args), "unsupported HotUpdateHostPack entry selection."),
        };
    }

    private static int RunAll(IEnumerable<Func<int>> entries)
    {
        foreach (var entry in entries)
        {
            Assert.Equal(0, entry(), "HotUpdateHostPack proof entry returned non-zero exit code.");
        }

        return 0;
    }
}
