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
        Priority = 2)]
    public static int Run()
    {
        ThreadSlot = 0;
        SharedTotal = 0;
        ThreadSlot += 1;

        lock (Gate)
        {
            SharedTotal += ThreadSlot;
        }

        Assert.Equal(1, SharedTotal);
        return 0;
    }
}
