using System;
using System.Collections.Immutable;
using Chaos.TestFramework;

public static partial class ExtensionsMarshalSubjects
{
    [Fact]
    public static void CustomEntrySubject_0()
    {
        var r = ImmutableArray.Create<byte>();
        Assert.AreEqual(0, r.Length);
    }

    [Fact]
    public static void CustomEntrySubject_1()
    {
        var r = ImmutableArray.Create<byte>((byte)42);
        Assert.AreEqual(1, r.Length);
    }

    [Fact]
    public static void CustomEntrySubject_2()
    {
        var r = ImmutableArray.Create<int>();
        Assert.AreEqual(0, r.Length);
    }

    [Fact]
    public static void CustomEntrySubject_3()
    {
        var r = ImmutableArray.Create<int>(42);
        Assert.AreEqual(1, r.Length);
    }
}
