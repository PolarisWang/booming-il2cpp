using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace CoreRuntimeBenchmarks;

internal static class NativeCallInteropBenchmarkMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern ulong GetTickCount64();

    internal static ulong RoundTripUInt64(ulong value)
    {
        return value;
    }
}

internal static class NativeCallInteropBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "native-call-interop-bench",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.NativeCallInterop,
        Requires = ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int checksum = 0;
        for (int i = 0; i < 32; i++)
        {
            _ = NativeCallInteropBenchmarkMethods.RoundTripUInt64(NativeCallInteropBenchmarkMethods.GetTickCount64());
            checksum += i;
        }

        return checksum;
    }
}
