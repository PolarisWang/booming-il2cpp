using Chaos.TestFramework;

namespace HotUpdateHostPack;

internal static class Program
{
    public static int Main(string[] args)
    {
        if (!ChaosSubjectEntryArguments.TryParse(args, out var selection) || selection.IsNone)
        {
            return HotUpdateSkeletonProofEntry.Run();
        }

        return selection switch
        {
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.HotUpdateSkeletonProof } => HotUpdateSkeletonProofEntry.Run(),
            _ => throw new ArgumentOutOfRangeException(nameof(args), "unsupported HotUpdateHostPack entry selection."),
        };
    }
}
