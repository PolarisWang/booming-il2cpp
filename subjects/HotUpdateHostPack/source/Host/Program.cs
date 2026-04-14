using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class Program
{
    private static readonly Func<int>[] DefaultProofEntries =
    [
        HotUpdateSkeletonProofEntry.Run,
        MetadataSupplementProofEntry.Run,
        MethodReplacementProofEntry.Run,
        SharedContractProofEntry.Run,
        VersionRollbackProofEntry.Run,
    ];

    private static readonly IReadOnlyDictionary<string, Func<int>> DeclaredProofEntriesBySourceEntry =
        new Dictionary<string, Func<int>>(StringComparer.Ordinal)
        {
            ["HotUpdateHostPack/HotUpdateSkeletonProofEntry::Run()"] = HotUpdateSkeletonProofEntry.Run,
            ["HotUpdateHostPack/MetadataSupplementProofEntry::Run()"] = MetadataSupplementProofEntry.Run,
            ["HotUpdateHostPack/MethodReplacementProofEntry::Run()"] = MethodReplacementProofEntry.Run,
            ["HotUpdateHostPack/SharedContractProofEntry::Run()"] = SharedContractProofEntry.Run,
            ["HotUpdateHostPack/VersionRollbackProofEntry::Run()"] = VersionRollbackProofEntry.Run,
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

    private static int RunSelectedProof(string sourceEntry)
    {
        if (DeclaredProofEntriesBySourceEntry.TryGetValue(sourceEntry, out var entry))
        {
            return entry();
        }

        throw new ArgumentOutOfRangeException(nameof(sourceEntry), "unsupported HotUpdateHostPack source entry.");
    }
}
