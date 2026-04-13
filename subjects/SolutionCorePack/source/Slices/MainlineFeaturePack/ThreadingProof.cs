using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class ThreadingProofEntry
{
    [ThreadStatic]
    private static int ThreadSlot;

    private static readonly object Gate = new();
    private static volatile int SharedTotal;

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "threading-proof",
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        ThreadSlot += 1;

        lock (Gate)
        {
            SharedTotal += ThreadSlot;
        }

        Console.WriteLine(SharedTotal);
        return 0;
    }
}
