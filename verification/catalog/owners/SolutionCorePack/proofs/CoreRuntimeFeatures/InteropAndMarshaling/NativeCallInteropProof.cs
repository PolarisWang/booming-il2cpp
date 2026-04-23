using Chaos.TestFramework;
using System.Reflection;
using System.Runtime.InteropServices;

namespace CoreRuntimeFeatures;

internal static class NativeCallInteropMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern ulong GetTickCount64();
}

internal static class NativeCallInteropProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "native-call-interop-proof",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.NativeCallInterop,
        Requires = ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Priority = 7)]
    public static int Run()
    {
        MethodInfo method =
            typeof(NativeCallInteropMethods).GetMethod(nameof(NativeCallInteropMethods.GetTickCount64), BindingFlags.Static | BindingFlags.NonPublic)
            ?? throw new InvalidOperationException("native call entry point missing");
        DllImportAttribute? attribute = method.GetCustomAttribute<DllImportAttribute>();

        Assert.NotNull(attribute);
        Assert.Equal("kernel32.dll", attribute.Value);
        if (OperatingSystem.IsWindows())
        {
            Assert.True(NativeCallInteropMethods.GetTickCount64() > 0);
        }

        return 0;
    }
}
