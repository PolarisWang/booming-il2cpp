using Chaos.TestFramework;

namespace MainlineFeaturePack;

internal static class DelegateChainProofEntry
{
    private static int s_counter;

    private static void Increment(string tag)
    {
        s_counter++;
        Console.WriteLine("delegate-invoked=" + tag);
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "delegate-chain-proof",
        Requires = ChaosRuntimeFeature.Delegate,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        s_counter = 0;
        Action<string> single = Increment;
        single("A");
        Console.WriteLine("delegate-single-count=" + s_counter);

        s_counter = 0;
        Action<string> multi = null!;
        multi += Increment;
        multi += Increment;
        multi += Increment;
        multi("B");
        Console.WriteLine("delegate-multi-count=" + s_counter);

        s_counter = 0;
        Action<string> chain = null!;
        Action<string> d1 = Increment;
        Action<string> d2 = Increment;
        chain += d1;
        chain += d2;
        chain += d1;
        chain -= d1;
        chain("C");
        Console.WriteLine("delegate-remove-count=" + s_counter);

        s_counter = 0;
        Action<string> evt = null!;
        evt += Increment;
        evt -= Increment;
        if (evt != null)
        {
            evt("D");
        }

        Console.WriteLine("delegate-empty-count=" + s_counter);
        return 0;
    }
}
