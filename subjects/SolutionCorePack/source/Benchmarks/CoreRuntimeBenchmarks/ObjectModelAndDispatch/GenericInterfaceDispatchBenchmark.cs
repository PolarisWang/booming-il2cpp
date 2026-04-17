using Chaos.TestFramework;

namespace CoreRuntimeBenchmarks;

internal interface IGenericDispatchValue<T>
{
    T Read();
}

internal sealed class GenericDispatchValue<T>(T value) : IGenericDispatchValue<T>
{
    private readonly T _value = value;

    public T Read()
    {
        return _value;
    }
}

internal static class GenericInterfaceDispatchBenchmarkEntry
{
    private const int IterationCount = 20_000;

    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "generic-interface-dispatch-bench",
        CapabilityFamily = ChaosCapabilityFamily.TypeSystemAndGenerics,
        Capability = ChaosCapabilityItem.GenericInterfaceDispatch,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int checksum = 0;
        for (int i = 0; i < IterationCount; i++)
        {
            IGenericDispatchValue<int> value = new GenericDispatchValue<int>(i);
            checksum += value.Read();
        }

        return checksum;
    }
}
