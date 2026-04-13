using Chaos.TestFramework;
using System.Runtime.InteropServices;

namespace MainlineFeaturePack;

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
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 2)]
    public static int Run()
    {
        var box = new ObjectOpsBox { Value = 42 };
        Console.WriteLine("obj-new=" + box.Value);
        object o = box;
        Console.WriteLine("obj-isinst=" + (o is ObjectOpsBox ? "yes" : "no"));
        var cast = (ObjectOpsBox)o;
        Console.WriteLine("obj-cast=" + cast.Value);
        IObjectOpsLabel label = new ObjectOpsTag();
        Console.WriteLine("obj-iface=" + label.Label());
        Console.WriteLine("obj-sizeof=" + Marshal.SizeOf<int>());
        return 0;
    }
}
