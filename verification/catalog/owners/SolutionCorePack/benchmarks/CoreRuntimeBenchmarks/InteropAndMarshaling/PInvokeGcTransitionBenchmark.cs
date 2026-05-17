using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace CoreRuntimeBenchmarks;

internal static class PInvokeGcTransitionBenchmarkMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern ulong GetTickCount64();

    [DllImport("kernel32.dll", ExactSpelling = true)]
    [SuppressGCTransition]
    internal static extern ulong GetTickCount64Suppressed();
}

internal static class PInvokeGcTransitionBaselineEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "pinvoke-gc-transition-baseline",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.NativeCallInterop,
        Requires = ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunBaseline()
    {
        int checksum = 0;
        for (int i = 0; i < 32; i++)
        {
            _ = PInvokeGcTransitionBenchmarkMethods.GetTickCount64();
            checksum += i;
        }

        return checksum;
    }
}

internal static class PInvokeGcTransitionSuppressedEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "pinvoke-gc-transition-suppressed",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.NativeCallInterop,
        Requires = ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Modes = ChaosExecutionMode.Managed,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunSuppressed()
    {
        int checksum = 0;
        for (int i = 0; i < 32; i++)
        {
            _ = PInvokeGcTransitionBenchmarkMethods.GetTickCount64Suppressed();
            checksum += i;
        }

        return checksum;
    }
}
