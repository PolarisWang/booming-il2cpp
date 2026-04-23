using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace CoreRuntimeBenchmarks;

internal delegate int DelegateCallbackInteropBenchmarkHandler(int value);

internal static class DelegateCallbackInteropBenchmarkEntry
{
    [ChaosBenchmark(
        ChaosBenchmarkCategory.RuntimeDispatch,
        ChaosMetric.WallClockUs,
        Alias = "delegate-callback-interop-bench",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.DelegateCallbackInterop,
        Requires = ChaosRuntimeFeature.Delegate | ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native,
        WarmupCount = 1,
        IterationCount = 2,
        InvocationCount = 1)]
    public static int RunWorkload()
    {
        DelegateCallbackInteropBenchmarkHandler handler = static value => value + 1;
        IntPtr pointer = Marshal.GetFunctionPointerForDelegate(handler);
        DelegateCallbackInteropBenchmarkHandler restored = Marshal.GetDelegateForFunctionPointer<DelegateCallbackInteropBenchmarkHandler>(pointer);
        int checksum = 0;

        for (int i = 0; i < 128; i++)
        {
            checksum += restored(i);
        }

        GC.KeepAlive(handler);
        return checksum % 10000;
    }
}
