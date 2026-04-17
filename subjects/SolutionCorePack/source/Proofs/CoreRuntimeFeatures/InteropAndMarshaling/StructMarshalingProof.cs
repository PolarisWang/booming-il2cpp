using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace CoreRuntimeFeatures;

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
internal struct StructMarshalingRecord
{
    public int Count;
    public double Ratio;
}

internal static class StructMarshalingProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "struct-marshaling-proof",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.StructMarshaling,
        Requires = ChaosRuntimeFeature.NativeInterop,
        Archetype = ChaosSolutionArchetype.ReferenceAssemblySolution,
        Priority = 7)]
    public static int Run()
    {
        var value = new StructMarshalingRecord
        {
            Count = 7,
            Ratio = 35.0,
        };

        IntPtr buffer = Marshal.AllocHGlobal(Marshal.SizeOf<StructMarshalingRecord>());
        Marshal.StructureToPtr(value, buffer, fDeleteOld: false);
        StructMarshalingRecord roundTrip = Marshal.PtrToStructure<StructMarshalingRecord>(buffer);

        Marshal.FreeHGlobal(buffer);
        Assert.Equal(42.0, roundTrip.Count + roundTrip.Ratio);

        return 0;
    }
}
