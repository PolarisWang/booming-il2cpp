// Handwritten custom entry for patch variant — Module.GetCustomAttributes(Type) at index 5.
// The auto-generator can't handle the metadata-vs-runtime type mismatch (System.Object[]
// in metadata vs IEnumerable<Attribute> at runtime).
using System;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
public static partial class ReflectionModulePatchEntry
{
    [System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
    [Fact]
    public static void CustomEntryMethod5()
    {
        var attrs = typeof(byte).Module.GetCustomAttributes(typeof(AssemblyDescriptionAttribute));
        var list = new List<Attribute>(attrs);
        Assert.IsFalse(list.Count == 0);
    }
}
