using System;
namespace ObjectOpsProof;
internal sealed class Box { public int Value; }
internal interface ILabel { string Label(); }
internal sealed class Tag : ILabel { public string Label() => "tag"; }
internal static class ObjectOpsProofEntry {
    public static int Run() {
        var box = new Box { Value = 42 };
        Console.WriteLine("obj-new=" + box.Value);      // 42
        object o = box;
        Console.WriteLine("obj-isinst=" + (o is Box ? "yes" : "no")); // yes
        var cast = (Box)o;
        Console.WriteLine("obj-cast=" + cast.Value);    // 42
        ILabel label = new Tag();
        Console.WriteLine("obj-iface=" + label.Label()); // tag
        Console.WriteLine("obj-sizeof=" + System.Runtime.InteropServices.Marshal.SizeOf<int>()); // 4
        return 0;
    }
}
internal static class Program { public static int Main() => ObjectOpsProofEntry.Run(); }
