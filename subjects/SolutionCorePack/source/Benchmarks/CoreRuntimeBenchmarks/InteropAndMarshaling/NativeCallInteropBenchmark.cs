using Chaos.TestFramework;
using System.Reflection;
using System.Runtime.InteropServices;

namespace CoreRuntimeBenchmarks;

internal static class NativeCallInteropBenchmarkMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern ulong GetTickCount64();
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
        if (OperatingSystem.IsWindows())
        {
            for (int i = 0; i < 32; i++)
            {
                checksum ^= (int)(NativeCallInteropBenchmarkMethods.GetTickCount64() & 0x7FFF);
            }

            return checksum;
        }

        MethodInfo method =
            typeof(NativeCallInteropBenchmarkMethods).GetMethod(nameof(NativeCallInteropBenchmarkMethods.GetTickCount64), BindingFlags.Static | BindingFlags.NonPublic)
            ?? throw new InvalidOperationException("native call benchmark entry missing");
        DllImportAttribute attribute = method.GetCustomAttribute<DllImportAttribute>()
            ?? throw new InvalidOperationException("DllImport attribute missing");
        return attribute.Value.Length;
    }
}
