using System;
using System.Collections.Immutable;
using Chaos.TestFramework;

public static partial class ImmutableListSubjects
{
    [Fact]
    public static void CustomEntrySubject_0()
    {
        var r = ImmutableList.Create<byte>();
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_1()
    {
        var r = ImmutableList.Create<byte>((byte)42);
        Assert.AreEqual(1, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_2()
    {
        var r = ImmutableList.Create<byte>((byte)42, (byte)43);
        Assert.AreEqual(2, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_3()
    {
        var r = ImmutableList.Create<int>();
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_4()
    {
        var r = ImmutableList.Create<int>(42);
        Assert.AreEqual(1, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_5()
    {
        var r = ImmutableList.Create<string>("hello");
        Assert.AreEqual(1, r.Count);
    }
}
