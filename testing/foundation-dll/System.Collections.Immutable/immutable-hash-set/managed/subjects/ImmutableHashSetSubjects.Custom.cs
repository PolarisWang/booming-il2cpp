using System;
using System.Collections.Immutable;
using Chaos.TestFramework;

public static partial class ImmutableHashSetSubjects
{
    [Fact]
    public static void CustomEntrySubject_0()
    {
        var r = ImmutableHashSet.Create<byte>();
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_1()
    {
        var r = ImmutableHashSet.Create<byte>((byte)42);
        Assert.AreEqual(1, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_2()
    {
        var r = ImmutableHashSet.Create<byte>((byte)42, (byte)43);
        Assert.AreEqual(2, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_3()
    {
        var r = ImmutableHashSet.Create<int>();
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_4()
    {
        var r = ImmutableHashSet.Create<int>(42);
        Assert.AreEqual(1, r.Count);
    }
}
