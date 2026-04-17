using Chaos.TestFramework;

namespace CoreRuntimeFeatures;

internal sealed class GcSensitiveFlowProbe
{
    public static int FinalizedCount;

    ~GcSensitiveFlowProbe()
    {
        Interlocked.Increment(ref FinalizedCount);
    }
}

internal static class GcSensitiveFlowProofEntry
{
    private static void AllocateProbe()
    {
        _ = new GcSensitiveFlowProbe();
    }

    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "gc-sensitive-flow-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.GcSensitiveFlow,
        Priority = 5)]
    public static int Run()
    {
        GcSensitiveFlowProbe.FinalizedCount = 0;
        AllocateProbe();

        for (int attempt = 0; attempt < 5 && GcSensitiveFlowProbe.FinalizedCount == 0; attempt++)
        {
            GC.Collect();
            GC.WaitForPendingFinalizers();
            GC.Collect();
        }

        var rooted = new GcSensitiveFlowProbe();
        GC.Collect();
        GC.WaitForPendingFinalizers();
        GC.KeepAlive(rooted);

        Assert.True(GcSensitiveFlowProbe.FinalizedCount >= 1);
        return 0;
    }
}
