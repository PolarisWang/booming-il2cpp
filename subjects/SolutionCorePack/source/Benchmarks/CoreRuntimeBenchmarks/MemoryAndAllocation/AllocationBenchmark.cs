using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal sealed class AllocationWorkItem
{
    public int Id;
    public string? Name;
    public double Value;
}

internal static class AllocationBenchmarkEntry
{
    private const int IterationCount = 1000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.Allocation,
        ChaosMetric.WallClockUs | ChaosMetric.ManagedAllocBytes,
        Alias = "allocation-bench",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.GcSensitiveFlow,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 3,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount * 10; i++)
        {
            var item = new AllocationWorkItem { Id = i, Name = "item" + i, Value = i * 1.5 };
            checksum += item.Id;
        }

        return checksum % 10000;
    }
}
