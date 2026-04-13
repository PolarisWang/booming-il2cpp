namespace MixedExecutionFeaturePack;

internal static class Program
{
    public static int Main(string[] args)
    {
        if (args.Length > 0 && string.Equals(args[0], "lowering-proof", StringComparison.Ordinal))
        {
            return InterpreterLoweringProofEntry.Run();
        }

        return MixedExecutionProofEntry.Run();
    }
}
