using Chaos.TestFramework;

namespace MixedExecutionFeaturePack;

internal static class Program
{
    public static int Main(string[] args)
    {
        if (!ChaosSubjectEntryArguments.TryParse(args, out var selection) || selection.IsNone)
        {
            return MixedExecutionProofEntry.Run();
        }

        return selection switch
        {
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.MixedExecutionProof } => MixedExecutionProofEntry.Run(),
            { EntryKind: ChaosSubjectEntryKind.Proof, EntrySlice: ChaosSubjectSlice.InterpreterLoweringProof } => InterpreterLoweringProofEntry.Run(),
            _ => throw new ArgumentOutOfRangeException(nameof(args), "unsupported MixedExecutionFeaturePack entry selection."),
        };
    }
}
