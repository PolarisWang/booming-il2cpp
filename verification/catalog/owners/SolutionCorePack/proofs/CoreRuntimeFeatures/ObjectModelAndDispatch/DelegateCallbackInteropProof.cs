using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace CoreRuntimeFeatures;

internal delegate int DelegateCallbackInteropHandler(int value);

internal static class DelegateCallbackInteropProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "delegate-callback-interop-proof",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.DelegateCallbackInterop,
        Requires = ChaosRuntimeFeature.Delegate | ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Priority = 7)]
    public static int Run()
    {
        DelegateCallbackInteropHandler handler = static value => value + 2;
        IntPtr functionPointer = Marshal.GetFunctionPointerForDelegate(handler);
        DelegateCallbackInteropHandler restored = Marshal.GetDelegateForFunctionPointer<DelegateCallbackInteropHandler>(functionPointer);

        Assert.Equal(42, restored(40));
        GC.KeepAlive(handler);
        return 0;
    }
}
