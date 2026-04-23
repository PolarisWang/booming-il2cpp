using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace CoreRuntimeBenchmarks;

internal static class StringAndUtf8MarshalingBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.Allocation,
        ChaosMetric.WallClockUs | ChaosMetric.ManagedAllocBytes,
        Alias = "string-utf8-marshaling-bench",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.StringAndUtf8Marshaling,
        Requires = ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        int checksum = 0;
        for (int i = 0; i < 64; i++)
        {
            IntPtr buffer = Marshal.StringToCoTaskMemUTF8("marshal-" + i);
            string? value = Marshal.PtrToStringUTF8(buffer);
            Marshal.FreeCoTaskMem(buffer);
            if (value is not null)
            {
                checksum += value.Length;
            }
        }

        return checksum % 10000;
    }
}
