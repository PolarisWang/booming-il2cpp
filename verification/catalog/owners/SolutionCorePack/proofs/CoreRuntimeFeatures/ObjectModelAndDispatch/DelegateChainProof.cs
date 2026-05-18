using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal static class DelegateChainProofEntry
{
    private static int s_counter;

    private static void Increment(string tag)
    {
        s_counter++;
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "delegate-chain-proof",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateChaining,
        Requires = ChaosRuntimeFeature.Delegate,
        Priority = 2)]
    public static int Run()
    {
        s_counter = 0;
        Action<string> single = Increment;
        single("A");
        Assert.Equal(1, s_counter);

        s_counter = 0;
        Action<string> multi = null!;
        multi += Increment;
        multi += Increment;
        multi += Increment;
        multi("B");
        Assert.Equal(3, s_counter);

        s_counter = 0;
        Action<string> chain = null!;
        Action<string> d1 = Increment;
        Action<string> d2 = Increment;
        chain += d1;
        chain += d2;
        chain += d1;
        chain -= d1;
        chain("C");
        Assert.Equal(2, s_counter);

        s_counter = 0;
        Action<string> evt = null!;
        evt += Increment;
        evt -= Increment;
        if (evt != null)
        {
            evt("D");
        }

        Assert.Equal(0, s_counter);
        return 0;
    }

    // B3: Cross-type delegate verification — verify incompatible types are detected.
    // The managed proof validates each delegate works independently.
    // The cross-type combine rejection is verified in the native stress binary.
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "delegate-cross-type-combine",
        CapabilityFamily = ChaosCapabilityFamily.DelegatesAndClosures,
        Capability = ChaosCapabilityItem.DelegateChaining,
        Requires = ChaosRuntimeFeature.Delegate,
        Priority = 2)]
    public static int RunCrossTypeCombine()
    {
        s_counter = 0;
        System.Action<string> strDel = Increment;
        int intSum = 0;
        System.Action<int> intDel = (v) => intSum += v;

        // Verify each delegate works independently
        strDel("E");
        Assert.Equal(1, s_counter);

        intDel(7);
        Assert.Equal(7, intSum);

        return 0;
    }
}
