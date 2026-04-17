using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace CoreRuntimeBenchmarks;

[StructLayout(LayoutKind.Sequential)]
internal struct StructMarshalingBenchmarkRecord
{
    public int Count;
    public double Ratio;
}

internal static class StructMarshalingBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.Allocation,
        ChaosMetric.WallClockUs | ChaosMetric.ManagedAllocBytes,
        Alias = "struct-marshaling-bench",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.StructMarshaling,
        Requires = ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        IntPtr buffer = Marshal.AllocHGlobal(Marshal.SizeOf<StructMarshalingBenchmarkRecord>());
        int checksum = 0;

        for (int i = 0; i < 64; i++)
        {
            var record = new StructMarshalingBenchmarkRecord
            {
                Count = i,
                Ratio = i + 0.5,
            };

            Marshal.StructureToPtr(record, buffer, fDeleteOld: false);
            StructMarshalingBenchmarkRecord roundTrip = Marshal.PtrToStructure<StructMarshalingBenchmarkRecord>(buffer);
            checksum += roundTrip.Count + (int)roundTrip.Ratio;
        }
        Marshal.FreeHGlobal(buffer);

        return checksum % 10000;
    }
}
