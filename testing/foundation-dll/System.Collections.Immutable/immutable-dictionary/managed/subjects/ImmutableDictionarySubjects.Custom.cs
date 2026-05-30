using System;
using System.Collections.Immutable;
using Chaos.TestFramework;

public static partial class ImmutableDictionarySubjects
{
    [Fact]
    public static void CustomEntrySubject_0()
    {
        var r = ImmutableDictionary.Create<string, int>();
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_1()
    {
        var r = ImmutableDictionary.Create<byte, byte>();
        Assert.AreEqual(0, r.Count);
    }

    [Fact]
    public static void CustomEntrySubject_2()
    {
        var r = ImmutableDictionary.Create<string, string>();
        Assert.AreEqual(0, r.Count);
    }
}
