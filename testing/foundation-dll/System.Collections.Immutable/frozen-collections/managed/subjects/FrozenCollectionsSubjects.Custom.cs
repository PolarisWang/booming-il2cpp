using System;
using System.Collections.Frozen;
using System.Collections.Generic;
using Chaos.TestFramework;

public static partial class FrozenCollectionsSubjects
{
    [Fact]
    public static void CustomEntrySubject_0()
    {
        var r = FrozenDictionary.ToFrozenDictionary(new Dictionary<string, int>());
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_1()
    {
        var r = FrozenDictionary.ToFrozenDictionary(new Dictionary<byte, byte>());
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_2()
    {
        var r = FrozenDictionary.ToFrozenDictionary(new Dictionary<string, string>());
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_3()
    {
        var r = FrozenSet.ToFrozenSet(new HashSet<byte>());
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_4()
    {
        var r = FrozenSet.ToFrozenSet(new byte[] { 42 });
        Assert.AreEqual(1, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_5()
    {
        var r = FrozenSet.ToFrozenSet(new byte[] { 42, 43 });
        Assert.AreEqual(2, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_6()
    {
        var r = FrozenSet.ToFrozenSet(new HashSet<int>());
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_7()
    {
        var r = FrozenSet.ToFrozenSet(new int[] { 42 });
        Assert.AreEqual(1, r.Count);
    }
}
