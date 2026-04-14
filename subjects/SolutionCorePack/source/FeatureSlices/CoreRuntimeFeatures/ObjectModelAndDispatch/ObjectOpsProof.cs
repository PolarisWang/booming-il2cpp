using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace CoreRuntimeFeatures;

internal sealed class ObjectOpsBox
{
    public int Value;
}

internal interface IObjectOpsLabel
{
    string Label();
}

internal sealed class ObjectOpsTag : IObjectOpsLabel
{
    public string Label()
    {
        return "tag";
    }
}

internal static class ObjectOpsProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.RuntimeContract,
        Alias = "object-ops-proof",
        CapabilityFamily = ChaosCapabilityFamily.RuntimeServices,
        Capability = ChaosCapabilityItem.RuntimeHelpers,
        Priority = 2)]
    public static int Run()
    {
        var box = new ObjectOpsBox { Value = 42 };
        Assert.Equal(42, box.Value);
        object o = box;
        Assert.True(o is ObjectOpsBox);
        var cast = (ObjectOpsBox)o;
        Assert.Equal(42, cast.Value);
        IObjectOpsLabel label = new ObjectOpsTag();
        Assert.Equal("tag", label.Label());
        Assert.Equal(4, Marshal.SizeOf<int>());
        return 0;
    }
}
